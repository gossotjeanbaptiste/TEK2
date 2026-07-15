from __future__ import annotations

import argparse
import os

import numpy as np
from PIL import Image, ImageFilter, ImageOps


def _local_contrast_mask(
    image: Image.Image,
    blur_radius: int,
    contrast_min: float,
    abs_min: float,
    roi_top_crop: float,
) -> np.ndarray:
    """
    Détecte les lignes blanches par contraste local.

    Principe : un pixel "ligne blanche" est à la fois :
      - brillant en absolu (abs_min)
      - beaucoup plus brillant que son voisinage immédiat (contrast_min)

    Cela exclut les zones uniformes (mur, sol clair) qui ont un faible
    contraste local, même si elles sont globalement claires.

    Le padding noir avant le flou évite l'effet de bord : PIL prolonge
    normalement avec le pixel de bord (blanc si la ligne est en bordure),
    ce qui fait monter la moyenne locale et masque la détection. Avec un
    padding noir on simule un fond sombre au-delà de l'image.
    """
    gray_pil = image.convert("L")
    gray = np.array(gray_pil, dtype=np.float32) / 255.0
    h, w = gray.shape

    # Padding noir avant BoxBlur pour éviter l'effet de bord sur les lignes
    # qui touchent les bords de l'image
    padded = ImageOps.expand(gray_pil, border=blur_radius, fill=0)
    blurred_padded = padded.filter(ImageFilter.BoxBlur(blur_radius))
    blurred_pil = blurred_padded.crop((blur_radius, blur_radius, blur_radius + w, blur_radius + h))
    local_mean = np.array(blurred_pil, dtype=np.float32) / 255.0

    contrast = gray - local_mean

    white = (gray >= abs_min) & (contrast >= contrast_min)

    if roi_top_crop > 0.0:
        cutoff = int(h * roi_top_crop)
        white[:cutoff, :] = False

    return white.astype(np.uint8)


def generate_mask(
    image: Image.Image,
    blur_radius: int,
    contrast_min: float,
    abs_min: float,
    roi_top_crop: float,
    dilate_passes: int,
) -> Image.Image:
    white = _local_contrast_mask(image, blur_radius, contrast_min, abs_min, roi_top_crop)
    mask = Image.fromarray(white * 255, mode="L")
    for _ in range(dilate_passes):
        mask = mask.filter(ImageFilter.MaxFilter(3))
    return mask


def run(
    in_dir: str,
    out_dir: str,
    blur_radius: int,
    contrast_min: float,
    abs_min: float,
    roi_top_crop: float,
    dilate_passes: int,
) -> None:
    os.makedirs(out_dir, exist_ok=True)

    extensions = (".png", ".jpg", ".jpeg")
    image_files = sorted(f for f in os.listdir(in_dir) if f.lower().endswith(extensions))

    if not image_files:
        print(f"Aucune image trouvée dans {in_dir}")
        return

    for filename in image_files:
        stem = os.path.splitext(filename)[0]
        in_path = os.path.join(in_dir, filename)
        out_path = os.path.join(out_dir, f"mask_{stem}.png")

        image = Image.open(in_path)
        mask = generate_mask(
            image,
            blur_radius=blur_radius,
            contrast_min=contrast_min,
            abs_min=abs_min,
            roi_top_crop=roi_top_crop,
            dilate_passes=dilate_passes,
        )
        mask.save(out_path)
        print(f"  {filename} → mask_{stem}.png")

    print(f"\n{len(image_files)} masques générés dans {out_dir}")
    print(
        "\nConseils si les lignes sont mal détectées :\n"
        "  - Trop de bruit : augmenter --contrast (défaut 0.30)\n"
        "  - Lignes manquantes : diminuer --contrast ou --abs-min\n"
        "  - Mur/fond inclus : augmenter --roi-top-crop (ex: 0.45)\n"
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description=(
            "Génère des masques binaires depuis de vraies photos en détectant\n"
            "les lignes blanches par contraste local (fonctionne sur tapis noir).\n"
            "Le mur et le fond uniforme sont automatiquement exclus.\n"
            "Nommage : mask_<nom_original>.png"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--in", dest="din", required=True, help="Dossier contenant les vraies photos")
    parser.add_argument("--out", dest="dout", required=True, help="Dossier de sortie pour les masques")
    parser.add_argument(
        "--blur-radius",
        type=int,
        default=30,
        help="Rayon du flou pour calculer la moyenne locale (défaut 30). "
             "Augmenter pour un contexte plus large (lignes fines/lointaines).",
    )
    parser.add_argument(
        "--contrast",
        type=float,
        default=0.22,
        help="Contraste minimum par rapport au voisinage (0.0-1.0, défaut 0.22). "
             "Diminuer si les lignes lointaines/fines sont manquantes. "
             "Augmenter si du bruit (reflets, sol clair) est détecté.",
    )
    parser.add_argument(
        "--abs-min",
        type=float,
        default=0.50,
        help="Luminosité absolue minimum (0.0-1.0, défaut 0.50). "
             "Élimine les zones sombres qui auraient un contraste artificiel.",
    )
    parser.add_argument(
        "--roi-top-crop",
        type=float,
        default=0.25,
        help="Fraction du haut de l'image à ignorer (0.0-1.0, défaut 0.25). "
             "Coupe le mur/fond en arrière-plan tout en conservant les lignes "
             "qui montent vers le haut de l'image. Ajuster selon l'angle caméra.",
    )
    parser.add_argument(
        "--dilate",
        type=int,
        default=3,
        help="Passes de dilatation pour relier les segments de lignes (défaut 3). "
             "Augmenter si les lignes sont discontinues.",
    )
    args = parser.parse_args()

    print(
        f"Paramètres : blur_radius={args.blur_radius}  contrast={args.contrast}"
        f"  abs_min={args.abs_min}  roi_top_crop={args.roi_top_crop}  dilate={args.dilate}"
    )
    run(
        args.din,
        args.dout,
        blur_radius=args.blur_radius,
        contrast_min=args.contrast,
        abs_min=args.abs_min,
        roi_top_crop=args.roi_top_crop,
        dilate_passes=args.dilate,
    )
