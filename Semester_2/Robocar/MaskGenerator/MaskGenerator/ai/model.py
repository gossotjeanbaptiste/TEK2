from __future__ import annotations

import os
from typing import Any

import joblib
import numpy as np
from PIL import Image
from sklearn.neural_network import MLPRegressor
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler

from raycast import raycast_from_mask


DEFAULT_IMAGE_SIZE = (160, 120)
DEFAULT_NUM_RAYS = 31
DEFAULT_FOV_DEG = 70.0
WHITE_THRESHOLD = 0.65

# Preset image sizes for different data sources
PRESET_IMAGE_SIZES = {
    "synthetic": (160, 120),
    "racing": (320, 240),
    "real": (640, 480),
}

# Seuil de blanc adapté à chaque source.
# "real" utilise un seuil plus élevé : les lignes blanches sur tapis noir
# sont très brillantes (>0.80) tandis que le mur/fond est à ~0.60-0.75.
PRESET_WHITE_THRESHOLDS = {
    "synthetic": 0.65,
    "racing": 0.70,
    "real": 0.82,
}

# Fraction du haut de l'image ignorée lors de l'extraction de features.
# Pour "real", coupe le mur/fond en arrière-plan (~35 % du haut).
PRESET_ROI_TOP_CROP = {
    "synthetic": 0.0,
    "racing": 0.0,
    "real": 0.25,
}


def resolve_image_size(source: str | None, requested: tuple[int, int] | None) -> tuple[int, int]:
    if requested is not None:
        return requested
    if source in PRESET_IMAGE_SIZES:
        return PRESET_IMAGE_SIZES[source]
    return DEFAULT_IMAGE_SIZE


def resolve_extract_params(
    source: str | None,
    white_threshold: float | None = None,
    roi_top_crop: float | None = None,
) -> tuple[float, float]:
    """Résout white_threshold et roi_top_crop depuis le preset source ou les valeurs explicites."""
    base_threshold = PRESET_WHITE_THRESHOLDS.get(source, WHITE_THRESHOLD) if source else WHITE_THRESHOLD
    base_roi = PRESET_ROI_TOP_CROP.get(source, 0.0) if source else 0.0
    return (
        white_threshold if white_threshold is not None else base_threshold,
        roi_top_crop if roi_top_crop is not None else base_roi,
    )


def _as_image(image_or_path: Any) -> Image.Image:
    if isinstance(image_or_path, Image.Image):
        return image_or_path
    if isinstance(image_or_path, np.ndarray):
        if image_or_path.ndim == 2:
            return Image.fromarray(image_or_path.astype(np.uint8), mode="L")
        if image_or_path.ndim == 3:
            return Image.fromarray(image_or_path.astype(np.uint8))
    if isinstance(image_or_path, str):
        return Image.open(image_or_path)
    raise TypeError(f"Unsupported image type: {type(image_or_path)!r}")


def _binary_mask(
    image_or_path: Any,
    size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
    white_threshold: float = WHITE_THRESHOLD,
    roi_top_crop: float = 0.0,
) -> np.ndarray:
    image = _as_image(image_or_path).convert("L").resize(size)
    gray = np.asarray(image, dtype=np.float32) / 255.0
    if roi_top_crop > 0.0:
        cutoff = int(gray.shape[0] * roi_top_crop)
        gray[:cutoff, :] = 0.0
    return (gray >= white_threshold).astype(np.uint8)


def extract_lane_features(
    image_or_path: Any,
    size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
    num_rays: int = DEFAULT_NUM_RAYS,
    fov_deg: float = DEFAULT_FOV_DEG,
    white_threshold: float = WHITE_THRESHOLD,
    roi_top_crop: float = 0.0,
) -> np.ndarray:
    """Convert a frame into compact features for the MLP."""

    binary = _binary_mask(image_or_path, size=size, white_threshold=white_threshold, roi_top_crop=roi_top_crop)
    height, width = binary.shape

    ray_features = raycast_from_mask(binary * 255, fov_deg=fov_deg, num_rays=num_rays)

    row_groups = np.array_split(np.arange(height), 3)
    col_groups = np.array_split(np.arange(width), 2)
    sector_features = [
        float(binary[np.ix_(rows, cols)].mean())
        for rows in row_groups
        for cols in col_groups
    ]
    global_coverage = float(binary.mean())

    return np.asarray(
        np.concatenate([
            ray_features.astype(np.float32),
            np.asarray(sector_features + [global_coverage], dtype=np.float32),
        ]),
        dtype=np.float32,
    )


def extract_lane_presence(
    mask_or_path: Any,
    size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
    roi_top_crop: float = 0.0,
) -> np.ndarray:
    """Extract left/right visibility labels from a mask."""

    # Les masques sont déjà binaires : seuil bas suffit
    binary = _binary_mask(mask_or_path, size=size, white_threshold=WHITE_THRESHOLD)
    height, width = binary.shape
    top_skip = max(height // 4, int(height * roi_top_crop))
    view = binary[top_skip:, :]

    left_visible = float(view[:, : width // 2].any())
    right_visible = float(view[:, width // 2 :].any())
    return np.asarray([left_visible, right_visible], dtype=np.float32)


def build_model(
    hidden_layers: tuple[int, ...] = (64, 32),
    seed: int = 42,
    max_iter: int = 300,
) -> Pipeline:
    return Pipeline(
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
                    max_iter=max_iter,
                    random_state=seed,
                    verbose=False,
                    early_stopping=True,
                    n_iter_no_change=12,
                    validation_fraction=0.15,
                ),
            ),
        ]
    )


def save_artifact(model: Pipeline, path: str, metadata: dict[str, Any]) -> None:
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    joblib.dump({"model": model, "metadata": metadata}, path)


def load_artifact(path: str) -> dict[str, Any]:
    artifact = joblib.load(path)
    if not isinstance(artifact, dict) or "model" not in artifact:
        raise RuntimeError(f"Invalid model artifact: {path}")
    return artifact


def predict_presence(
    model: Pipeline,
    image_or_path: Any,
    size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
    white_threshold: float = WHITE_THRESHOLD,
    roi_top_crop: float = 0.0,
) -> np.ndarray:
    features = extract_lane_features(
        image_or_path,
        size=size,
        white_threshold=white_threshold,
        roi_top_crop=roi_top_crop,
    )
    scores = np.asarray(model.predict(np.asarray([features], dtype=np.float32))[0], dtype=np.float32)
    return np.clip(scores, 0.0, 1.0)
