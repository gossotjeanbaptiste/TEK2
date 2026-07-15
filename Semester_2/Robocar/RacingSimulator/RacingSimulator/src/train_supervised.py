#!/usr/bin/env python3
import argparse
import glob
import json
import os
from dataclasses import dataclass
from typing import Any

import joblib
import numpy as np
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPRegressor
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)


@dataclass
class Dataset:
    x: np.ndarray
    y: np.ndarray
    n_lines: int
    source_files: list[str]


def _load_jsonl(dataset_paths: list[str], max_samples: int, behavior: str | None) -> Dataset:
    xs: list[list[float]] = []
    ys: list[list[float]] = []
    n_lines = 0
    source_files: list[str] = []

    for dataset_path in dataset_paths:
        source_files.append(dataset_path)
        with open(dataset_path, "r", encoding="utf-8") as f:
            for line in f:
                n_lines += 1
                line = line.strip()
                if not line:
                    continue

                payload: dict[str, Any] = json.loads(line)
                if behavior is not None and payload.get("behavior") != behavior:
                    continue

                obs = payload.get("observation")
                action = payload.get("action")
                action_type = payload.get("action_type")

                if action_type != "continuous":
                    continue
                if not isinstance(obs, list) or not isinstance(action, list):
                    continue

                xs.append([float(v) for v in obs])
                ys.append([float(v) for v in action])

                if 0 < max_samples <= len(xs):
                    break

        if 0 < max_samples <= len(xs):
            break

    if not xs:
        raise RuntimeError(
            "No usable samples found. Ensure dataset has continuous actions and non-empty observation/action arrays."
        )

    obs_dim = len(xs[0])
    action_dim = len(ys[0])
    if any(len(v) != obs_dim for v in xs):
        raise RuntimeError("Inconsistent observation dimensions in dataset")
    if any(len(v) != action_dim for v in ys):
        raise RuntimeError("Inconsistent action dimensions in dataset")

    x = np.asarray(xs, dtype=np.float32)
    y = np.asarray(ys, dtype=np.float32)
    return Dataset(x=x, y=y, n_lines=n_lines, source_files=source_files)


def _resolve_dataset_paths(dataset_paths: list[str] | None, dataset_globs: list[str] | None) -> list[str]:
    resolved: list[str] = []
    if dataset_paths:
        resolved.extend(dataset_paths)
    if dataset_globs:
        for pattern in dataset_globs:
            resolved.extend(sorted(glob.glob(pattern)))

    if not resolved:
        resolved = [os.path.join(PROJECT_ROOT, "data", "datasets", "manual_samples_v2.jsonl")]

    uniq: list[str] = []
    seen: set[str] = set()
    for p in resolved:
        ap = os.path.abspath(p)
        if ap in seen:
            continue
        if not os.path.isfile(ap):
            raise FileNotFoundError(f"Dataset file not found: {ap}")
        seen.add(ap)
        uniq.append(ap)
    return uniq


def main() -> int:
    parser = argparse.ArgumentParser(description="Train a supervised driving model from JSONL data")
    parser.add_argument(
        "--dataset-path",
        action="append",
        default=None,
        help="Input JSONL dataset (repeat option for multiple files)",
    )
    parser.add_argument(
        "--dataset-glob",
        action="append",
        default=None,
        help="Glob pattern for dataset files (repeat option), e.g. ./dataset/manual_*.jsonl",
    )
    parser.add_argument("--behavior", default=None, help="Optional behavior name filter")
    parser.add_argument("--max-samples", type=int, default=0, help="Max samples to load (0 = all)")
    parser.add_argument("--test-size", type=float, default=0.2, help="Validation split ratio")
    parser.add_argument("--seed", type=int, default=42, help="Random seed")
    parser.add_argument("--hidden", default="128,128", help="MLP hidden layers, e.g. 128,128")
    parser.add_argument("--max-iter", type=int, default=200, help="Max MLP training iterations")
    parser.add_argument(
        "--output-model",
        default=os.path.join(PROJECT_ROOT, "models", "policy_mlp.joblib"),
        help="Saved model path",
    )
    args = parser.parse_args()

    hidden_layers = tuple(int(v.strip()) for v in args.hidden.split(",") if v.strip())
    if not hidden_layers:
        raise RuntimeError("--hidden must contain at least one layer size")

    dataset_paths = _resolve_dataset_paths(args.dataset_path, args.dataset_glob)

    dataset = _load_jsonl(
        dataset_paths=dataset_paths,
        max_samples=args.max_samples,
        behavior=args.behavior,
    )

    x_train, x_val, y_train, y_val = train_test_split(
        dataset.x,
        dataset.y,
        test_size=args.test_size,
        random_state=args.seed,
        shuffle=True,
    )

    model = Pipeline(
        [
            ("scaler", StandardScaler()),
            (
                "mlp",
                MLPRegressor(
                    hidden_layer_sizes=hidden_layers,
                    activation="relu",
                    solver="adam",
                    alpha=1e-4,
                    learning_rate_init=1e-3,
                    max_iter=args.max_iter,
                    random_state=args.seed,
                    verbose=True,
                    early_stopping=True,
                    n_iter_no_change=10,
                    validation_fraction=0.1,
                ),
            ),
        ]
    )

    print(f"Loaded {len(dataset.x)} samples from {len(dataset.source_files)} file(s) (raw lines: {dataset.n_lines})")
    for p in dataset.source_files:
        print(f"- {p}")
    print(f"obs_dim={dataset.x.shape[1]}, action_dim={dataset.y.shape[1]}")
    print(f"Train={len(x_train)} | Val={len(x_val)}")
    print("Training started...")

    model.fit(x_train, y_train)

    y_pred = model.predict(x_val)
    mse = mean_squared_error(y_val, y_pred)
    mae = mean_absolute_error(y_val, y_pred)
    r2 = r2_score(y_val, y_pred)

    print("Training finished")
    print(f"Validation MSE: {mse:.6f} (lower is better)")
    print(f"Validation MAE: {mae:.6f} (lower is better)")
    print(f"Validation R2 : {r2:.6f} (higher is better)")

    out_path = os.path.abspath(args.output_model)
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    artifact = {
        "model": model,
        "obs_dim": int(dataset.x.shape[1]),
        "action_dim": int(dataset.y.shape[1]),
        "seed": args.seed,
        "hidden_layers": hidden_layers,
    }
    joblib.dump(artifact, out_path)
    print(f"Saved model: {out_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
