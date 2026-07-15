from __future__ import annotations

import argparse
import json
import math
import os
import random

import numpy as np
from PIL import Image, ImageDraw, ImageEnhance


def _make_background(width: int, height: int) -> Image.Image:
    base = random.randint(32, 58)
    arr = np.full((height, width, 3), base, dtype=np.float32)

    gradient = np.linspace(18, -10, height, dtype=np.float32)[:, None, None]
    arr += gradient
    arr += np.random.randn(height, width, 3).astype(np.float32) * 7.0

    return Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8), mode="RGB")


def _track_geometry(width: int, height: int, n_points: int = 24) -> tuple[list[tuple[int, int]], list[tuple[int, int]], bool, bool]:
    ys = np.linspace(height - 1, int(height * 0.15), n_points).astype(int)
    t = np.linspace(0.0, 1.0, n_points)

    center_offset = random.uniform(-width * 0.68, width * 0.68)
    sway = random.uniform(-width * 0.3, width * 0.3)
    curve = random.uniform(-width * 0.55, width * 0.55)
    lane_half_bottom = random.uniform(width * 0.06, width * 0.15)
    lane_half_top = lane_half_bottom * random.uniform(0.15, 0.4)

    lane_half = lane_half_top + (lane_half_bottom - lane_half_top) * (1.0 - t)
    center_x = (
        width / 2.0
        + center_offset
        + sway * np.sin(t * math.pi)
        + curve * (t - 0.5) ** 2
    )

    left_points: list[tuple[int, int]] = []
    right_points: list[tuple[int, int]] = []
    left_visible = False
    right_visible = False

    for x_center, x_half, y in zip(center_x, lane_half, ys):
        left_x = int(round(x_center - x_half))
        right_x = int(round(x_center + x_half))
        left_points.append((left_x, int(y)))
        right_points.append((right_x, int(y)))

        if 0 <= left_x < width:
            left_visible = True
        if 0 <= right_x < width:
            right_visible = True

    return left_points, right_points, left_visible, right_visible


def _draw_track(width: int, height: int, line_thickness: int = 7) -> tuple[Image.Image, Image.Image, dict[str, bool]]:
    background = _make_background(width, height)
    image = background.copy()
    mask = Image.new("L", (width, height), 0)
    draw_image = ImageDraw.Draw(image)
    draw_mask = ImageDraw.Draw(mask)

    left_points, right_points, left_visible, right_visible = _track_geometry(width, height)

    road_polygon = left_points + list(reversed(right_points))
    draw_image.polygon(road_polygon, fill=(42, 42, 42))

    edge_fill = (235, 235, 235)
    draw_image.line(left_points, fill=edge_fill, width=line_thickness)
    draw_image.line(right_points, fill=edge_fill, width=line_thickness)
    draw_mask.line(left_points, fill=255, width=line_thickness)
    draw_mask.line(right_points, fill=255, width=line_thickness)

    image = ImageEnhance.Brightness(image).enhance(random.uniform(0.85, 1.15))
    image = ImageEnhance.Contrast(image).enhance(random.uniform(0.95, 1.2))

    arr = np.asarray(image, dtype=np.int16)
    arr += (np.random.randn(*arr.shape) * 4).astype(np.int16)
    image = Image.fromarray(np.clip(arr, 0, 255).astype(np.uint8), mode="RGB")

    return image, mask, {"left_visible": left_visible, "right_visible": right_visible}


def generate_dataset(
    out_dir: str,
    n: int = 1000,
    size: tuple[int, int] = (320, 240),
    line_thickness: int = 7,
) -> None:
    width, height = size
    images_dir = os.path.join(out_dir, "images")
    masks_dir = os.path.join(out_dir, "masks")
    os.makedirs(images_dir, exist_ok=True)
    os.makedirs(masks_dir, exist_ok=True)

    annotations_path = os.path.join(out_dir, "annotations.jsonl")
    with open(annotations_path, "w", encoding="utf-8") as annotations_file:
        for index in range(n):
            image, mask, labels = _draw_track(width, height, line_thickness=line_thickness)

            image_name = f"image_{index:05d}.png"
            mask_name = f"mask_{index:05d}.png"
            image.save(os.path.join(images_dir, image_name))
            mask.save(os.path.join(masks_dir, mask_name))

            annotations_file.write(
                json.dumps(
                    {
                        "image": image_name,
                        "mask": mask_name,
                        **labels,
                    },
                    separators=(",", ":"),
                )
                + "\n"
            )

    print(f"Generated {n} samples in {out_dir}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a synthetic circuit dataset with two white lane lines")
    parser.add_argument("--out", default="dataset", help="Output directory")
    parser.add_argument("--n", type=int, default=1000, help="Number of samples")
    parser.add_argument("--w", type=int, default=320, help="Image width")
    parser.add_argument("--h", type=int, default=240, help="Image height")
    parser.add_argument("--thickness", type=int, default=7, help="Lane line thickness")
    args = parser.parse_args()

    generate_dataset(args.out, n=args.n, size=(args.w, args.h), line_thickness=args.thickness)
