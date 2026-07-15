from __future__ import annotations

import os
from dataclasses import dataclass

import numpy as np

from model import DEFAULT_IMAGE_SIZE, WHITE_THRESHOLD, extract_lane_features, extract_lane_presence


def _image_key(path: str) -> str:
    base = os.path.splitext(os.path.basename(path))[0]
    if base.startswith("image_"):
        return base[len("image_") :]
    return base


def _mask_key(path: str) -> str:
    base = os.path.splitext(os.path.basename(path))[0]
    if base.startswith("mask_"):
        return base[len("mask_") :]
    return base


@dataclass
class LaneSample:
    image_path: str
    mask_path: str


class LanePresenceDataset:
    """Paired image/mask dataset for lane presence classification."""

    def __init__(
        self,
        image_dir: str,
        mask_dir: str,
        size: tuple[int, int] = DEFAULT_IMAGE_SIZE,
        white_threshold: float = WHITE_THRESHOLD,
        roi_top_crop: float = 0.0,
    ):
        self.image_dir = image_dir
        self.mask_dir = mask_dir
        self.size = size
        self.white_threshold = white_threshold
        self.roi_top_crop = roi_top_crop

        image_files = sorted(
            [
                os.path.join(image_dir, entry)
                for entry in os.listdir(image_dir)
                if entry.lower().endswith((".png", ".jpg", ".jpeg"))
            ]
        )
        mask_files = sorted(
            [
                os.path.join(mask_dir, entry)
                for entry in os.listdir(mask_dir)
                if entry.lower().endswith((".png", ".jpg", ".jpeg"))
            ]
        )

        image_map = {_image_key(path): path for path in image_files}
        mask_map = {_mask_key(path): path for path in mask_files}
        common_keys = sorted(set(image_map) & set(mask_map))

        self.samples = [LaneSample(image_map[key], mask_map[key]) for key in common_keys]

    def __len__(self) -> int:
        return len(self.samples)

    def __getitem__(self, idx: int) -> tuple[np.ndarray, np.ndarray, str]:
        sample = self.samples[idx]
        features = extract_lane_features(
            sample.image_path,
            size=self.size,
            white_threshold=self.white_threshold,
            roi_top_crop=self.roi_top_crop,
        )
        labels = extract_lane_presence(
            sample.mask_path,
            size=self.size,
            roi_top_crop=self.roi_top_crop,
        )
        return features, labels, os.path.basename(sample.image_path)
