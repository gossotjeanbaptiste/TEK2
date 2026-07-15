"""Smoke test for the new lane-presence pipeline."""

from __future__ import annotations

import os
import shutil
import tempfile

from generate_synthetic_dataset import generate_dataset
from infer_unity import infer
from train_unity import train


def main() -> None:
    out = tempfile.mkdtemp(prefix="mask_generator_smoke_")
    print(f"Smoke test workspace: {out}")

    generate_dataset(out, n=80, size=(320, 240), line_thickness=7)

    model_path = os.path.join(out, "lane_presence.joblib")
    train(
        os.path.join(out, "images"),
        os.path.join(out, "masks"),
        out=model_path,
        hidden=(64, 32),
        test_size=0.25,
        image_size=(160, 120),
        max_iter=300,
    )

    infer(
        model_path,
        os.path.join(out, "images"),
        os.path.join(out, "masks"),
        out_dir=os.path.join(out, "preds"),
        image_size=(160, 120),
    )


if __name__ == "__main__":
    main()
