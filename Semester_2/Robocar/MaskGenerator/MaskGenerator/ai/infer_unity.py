from __future__ import annotations

import argparse
import json
import os

import numpy as np

from model import DEFAULT_IMAGE_SIZE, WHITE_THRESHOLD, extract_lane_presence, load_artifact, predict_presence


def infer(
    model_path: str,
    data_in: str,
    data_mask: str | None = None,
    out_dir: str = "preds",
    image_size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
) -> None:
    os.makedirs(out_dir, exist_ok=True)

    artifact = load_artifact(model_path)
    model = artifact["model"]
    metadata = artifact.get("metadata", {})

    # Utilise les paramètres stockés à l'entraînement pour rester cohérent
    stored_size = metadata.get("image_size")
    if stored_size is not None:
        image_size = tuple(stored_size)

    white_threshold = float(metadata.get("white_threshold", WHITE_THRESHOLD))
    roi_top_crop = float(metadata.get("roi_top_crop", 0.0))

    image_files = sorted(
        [
            os.path.join(data_in, entry)
            for entry in os.listdir(data_in)
            if entry.lower().endswith((".png", ".jpg", ".jpeg"))
        ]
    )

    report: list[dict[str, object]] = []
    y_true: list[np.ndarray] = []
    y_pred: list[np.ndarray] = []

    mask_map: dict[str, str] = {}
    if data_mask:
        mask_map = {
            os.path.splitext(entry)[0].replace("mask_", ""): os.path.join(data_mask, entry)
            for entry in os.listdir(data_mask)
            if entry.lower().endswith((".png", ".jpg", ".jpeg"))
        }

    for image_path in image_files:
        probabilities = predict_presence(
            model,
            image_path,
            size=image_size,
            white_threshold=white_threshold,
            roi_top_crop=roi_top_crop,
        )
        prediction = (probabilities >= 0.5).astype(int)

        row: dict[str, object] = {
            "image": os.path.basename(image_path),
            "left_probability": float(probabilities[0]),
            "right_probability": float(probabilities[1]),
            "left_present": bool(prediction[0]),
            "right_present": bool(prediction[1]),
        }

        key = os.path.splitext(os.path.basename(image_path))[0].replace("image_", "")
        if key in mask_map:
            truth = extract_lane_presence(mask_map[key], size=image_size, roi_top_crop=roi_top_crop)
            row["gt_left_present"] = bool(truth[0])
            row["gt_right_present"] = bool(truth[1])
            y_true.append(truth)
            y_pred.append(probabilities)

        report.append(row)

    report_path = os.path.join(out_dir, "predictions.jsonl")
    with open(report_path, "w", encoding="utf-8") as report_file:
        for row in report:
            report_file.write(json.dumps(row, separators=(",", ":")) + "\n")

    print(f"Saved predictions to {report_path}")
    print(f"Loaded artifact metadata: {metadata}")

    if y_true:
        y_true_arr = np.asarray(y_true, dtype=np.float32)
        y_pred_arr = np.clip(np.asarray(y_pred, dtype=np.float32), 0.0, 1.0)
        y_pred_binary = (y_pred_arr >= 0.5).astype(np.float32)
        lane_accuracy = float(np.mean(y_pred_binary == y_true_arr))
        sample_accuracy = float(np.mean(np.all(y_pred_binary == y_true_arr, axis=1)))
        print(f"Lane accuracy: {lane_accuracy:.3f}")
        print(f"Sample accuracy: {sample_accuracy:.3f}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run lane-presence inference on a folder of images")
    parser.add_argument("--model", required=True, help="Path to the trained joblib model")
    parser.add_argument("--in", dest="din", required=True, help="Input image directory")
    parser.add_argument("--mask", dest="dmask", default=None, help="Optional mask directory for evaluation")
    parser.add_argument("--out", default="preds", help="Output directory")
    parser.add_argument(
        "--source",
        choices=["synthetic", "racing", "real"],
        default=None,
        help="Ignoré si le modèle contient déjà les paramètres (cas normal). "
             "Utile uniquement pour forcer des paramètres sur un ancien modèle.",
    )
    parser.add_argument("--w", type=int, default=None, help="Feature image width (overrides source)")
    parser.add_argument("--h", type=int, default=None, help="Feature image height (overrides source)")
    args = parser.parse_args()

    requested = None
    if args.w is not None and args.h is not None:
        requested = (args.w, args.h)
    from model import resolve_image_size
    image_size = resolve_image_size(args.source, requested)

    infer(args.model, args.din, args.dmask, out_dir=args.out, image_size=image_size)
