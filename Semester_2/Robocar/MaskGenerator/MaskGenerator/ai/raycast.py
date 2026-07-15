import math
import numpy as np


def raycast_from_mask(mask, fov_deg=60, num_rays=31, max_distance=None):
    """
    Fast 2D raycast from bottom-center of the mask image.

    mask: 2D numpy array (H, W), values 0 or >0
    returns: distances array of length num_rays in pixels (or 1.0 normalized if max_distance provided)
    """
    h, w = mask.shape[:2]
    if max_distance is None:
        max_distance = math.hypot(w, h)

    cx = w / 2.0
    cy = h - 1  # bottom row

    angles = np.linspace(-fov_deg / 2.0, fov_deg / 2.0, num_rays) * math.pi / 180.0
    dists = np.full(num_rays, max_distance, dtype=float)

    ys, xs = np.indices((h, w))

    for i, a in enumerate(angles):
        dx = math.cos(a)
        dy = -math.sin(a)  # negative because y grows downward in image coords
        # step along ray with integer sampling
        for t in range(1, int(max_distance)):
            x = int(round(cx + dx * t))
            y = int(round(cy + dy * t))
            if x < 0 or x >= w or y < 0 or y >= h:
                break
            if mask[y, x] > 0:
                dists[i] = t
                break

    # normalize to 0..1
    return dists / max_distance


if __name__ == '__main__':
    import sys
    from PIL import Image
    import numpy as np

    if len(sys.argv) < 2:
        print('Usage: raycast.py mask.png')
        sys.exit(1)
    m = np.array(Image.open(sys.argv[1]).convert('L'))
    d = raycast_from_mask(m, fov_deg=60, num_rays=31)
    print(d)
