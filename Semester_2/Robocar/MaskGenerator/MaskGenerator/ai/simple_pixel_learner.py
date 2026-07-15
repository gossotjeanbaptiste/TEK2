from __future__ import annotations

import argparse
import os

import numpy as np
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import f1_score
from sklearn.model_selection import train_test_split
from sklearn.multioutput import MultiOutputClassifier
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler

from model import DEFAULT_IMAGE_SIZE
from unity_dataset import LanePresenceDataset


def train_baseline(image_dir: str, mask_dir: str, out_dir: str = "preds_np", seed: int = 42) -> dict[str, float]:
    dataset = LanePresenceDataset(image_dir, mask_dir, size=DEFAULT_IMAGE_SIZE)
    features = []
    labels = []
    for index in range(len(dataset)):
        x, y, _ = dataset[index]
        features.append(x)
        labels.append(y)

    x = np.asarray(features, dtype=np.float32)
    y = np.asarray(labels, dtype=np.float32)
    x_train, x_val, y_train, y_val = train_test_split(x, y, test_size=0.2, random_state=seed, shuffle=True)

    model = Pipeline(
        [
            ("scaler", StandardScaler()),
            (
                "clf",
                MultiOutputClassifier(
                    LogisticRegression(max_iter=1000, random_state=seed)
                ),
            ),
        ]
    )
    model.fit(x_train, y_train)

    val_pred = np.asarray(model.predict(x_val), dtype=np.float32)
    lane_accuracy = float(np.mean(val_pred == y_val))
    sample_accuracy = float(np.mean(np.all(val_pred == y_val, axis=1)))
    f1_left = float(f1_score(y_val[:, 0], val_pred[:, 0], zero_division=0))
    f1_right = float(f1_score(y_val[:, 1], val_pred[:, 1], zero_division=0))

    os.makedirs(out_dir, exist_ok=True)
    print(f"Baseline lane accuracy: {lane_accuracy:.3f}")
    print(f"Baseline sample accuracy: {sample_accuracy:.3f}")
    print(f"Baseline F1 left/right: {f1_left:.3f} / {f1_right:.3f}")
    return {
        "lane_accuracy": lane_accuracy,
        "sample_accuracy": sample_accuracy,
        "f1_left": f1_left,
        "f1_right": f1_right,
    }


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Baseline classifier for lane presence")
    parser.add_argument("--in", dest="din", required=True, help="Input image directory")
    parser.add_argument("--mask", dest="dmask", required=True, help="Input mask directory")
    args = parser.parse_args()

    train_baseline(args.din, args.dmask)
