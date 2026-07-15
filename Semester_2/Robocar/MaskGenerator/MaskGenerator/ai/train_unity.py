from __future__ import annotations

import argparse
import os

import numpy as np
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split

from model import (
    DEFAULT_IMAGE_SIZE,
    WHITE_THRESHOLD,
    build_model,
    resolve_extract_params,
    resolve_image_size,
    save_artifact,
)
from unity_dataset import LanePresenceDataset


def train(
    data_in: str,
    data_mask: str,
    epochs: int = 1,
    batch_size: int = 8,
    lr: float = 1e-3,
    out: str = "MaskGenerator/ai/lane_presence.joblib",
    hidden: tuple[int, ...] = (64, 32),
    test_size: float = 0.2,
    seed: int = 42,
    image_size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
    max_iter: int = 300,
    white_threshold: float = WHITE_THRESHOLD,
    roi_top_crop: float = 0.0,
) -> None:
    _ = epochs, batch_size, lr

    dataset = LanePresenceDataset(
        data_in,
        data_mask,
        size=image_size,
        white_threshold=white_threshold,
        roi_top_crop=roi_top_crop,
    )
    if len(dataset) < 4:
        raise RuntimeError("Need at least 4 samples to train the model")

    features = []
    labels = []
    for index in range(len(dataset)):
        x, y, _name = dataset[index]
        features.append(x)
        labels.append(y)

    x = np.asarray(features, dtype=np.float32)
    y = np.asarray(labels, dtype=np.float32)

    x_train, x_val, y_train, y_val = train_test_split(
        x,
        y,
        test_size=test_size,
        random_state=seed,
        shuffle=True,
    )

    model = build_model(hidden_layers=hidden, seed=seed, max_iter=max_iter)
    print(f"Training on {len(x_train)} samples, validating on {len(x_val)} samples")
    print(f"  white_threshold={white_threshold:.2f}  roi_top_crop={roi_top_crop:.2f}")
    model.fit(x_train, y_train)

    val_pred = np.clip(model.predict(x_val), 0.0, 1.0)
    val_binary = (val_pred >= 0.5).astype(np.float32)
    mse = mean_squared_error(y_val, val_pred)
    sample_accuracy = float(np.mean(np.all(val_binary == y_val, axis=1)))
    lane_accuracy = float(np.mean(val_binary == y_val))

    metadata = {
        "image_size": list(image_size),
        "feature_count": int(x.shape[1]),
        "hidden_layers": list(hidden),
        "seed": seed,
        "train_samples": int(len(x_train)),
        "val_samples": int(len(x_val)),
        "mse": float(mse),
        "sample_accuracy": sample_accuracy,
        "lane_accuracy": lane_accuracy,
        "source_images": os.path.abspath(data_in),
        "source_masks": os.path.abspath(data_mask),
        "white_threshold": white_threshold,
        "roi_top_crop": roi_top_crop,
    }

    save_artifact(model, out, metadata)
    print(f"Saved model to {out}")
    print(f"Validation MSE: {mse:.6f}")
    print(f"Lane accuracy: {lane_accuracy:.3f}")
    print(f"Sample accuracy: {sample_accuracy:.3f}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Train the lane-presence MLP")
    parser.add_argument("--in", dest="din", required=True, help="Input image directory")
    parser.add_argument("--mask", dest="dmask", required=True, help="Input mask directory")
    parser.add_argument("--epochs", type=int, default=1, help="Kept for compatibility")
    parser.add_argument("--batch", type=int, default=8, help="Kept for compatibility")
    parser.add_argument("--out", default="MaskGenerator/ai/lane_presence.joblib", help="Output joblib path")
    parser.add_argument("--hidden", default="64,32", help="Hidden layers, e.g. 64,32")
    parser.add_argument("--max-iter", type=int, default=300, help="MLP training iterations")
    parser.add_argument("--test-size", type=float, default=0.2, help="Validation split ratio")
    parser.add_argument(
        "--source",
        choices=["synthetic", "racing", "real"],
        default=None,
        help="Dataset source type : applique des valeurs par défaut adaptées (taille, seuil, ROI).",
    )
    parser.add_argument("--w", type=int, default=None, help="Feature image width (overrides source)")
    parser.add_argument("--h", type=int, default=None, help="Feature image height (overrides source)")
    parser.add_argument(
        "--white-threshold",
        type=float,
        default=None,
        help="Seuil de luminosité pour pixel blanc (0.0-1.0). "
             "Défaut : 0.65 (synthetic), 0.70 (racing), 0.82 (real).",
    )
    parser.add_argument(
        "--roi-top-crop",
        type=float,
        default=None,
        help="Fraction du haut de l'image ignorée (0.0-1.0). "
             "Défaut : 0.0 (synthetic/racing), 0.40 (real). "
             "Permet d'exclure le mur/fond en arrière-plan.",
    )
    args = parser.parse_args()

    hidden_layers = tuple(int(v.strip()) for v in args.hidden.split(",") if v.strip())

    requested = None
    if args.w is not None and args.h is not None:
        requested = (args.w, args.h)
    image_size = resolve_image_size(args.source, requested)

    white_threshold, roi_top_crop = resolve_extract_params(
        args.source,
        white_threshold=args.white_threshold,
        roi_top_crop=args.roi_top_crop,
    )

    train(
        args.din,
        args.dmask,
        epochs=args.epochs,
        batch_size=args.batch,
        out=args.out,
        hidden=hidden_layers,
        test_size=args.test_size,
        image_size=image_size,
        max_iter=args.max_iter,
        white_threshold=white_threshold,
        roi_top_crop=roi_top_crop,
    )
