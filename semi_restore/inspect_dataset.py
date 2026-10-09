from pathlib import Path
from PIL import Image
import numpy as np


DEGRADED_DIR = Path("data/train/degraded")
GT_DIR = Path("data/train/ground_truth")

VALID_EXTENSIONS = {
    ".png",
    ".jpg",
    ".jpeg",
    ".tif",
    ".tiff",
}


def inspect_folder(folder):
    files = sorted(
        [
            p for p in folder.iterdir()
            if p.suffix.lower() in VALID_EXTENSIONS
        ]
    )

    print(f"\nFolder: {folder}")
    print(f"Number of images: {len(files)}")

    if not files:
        return

    print("\nFirst 5 images:")

    for path in files[:5]:

        image = Image.open(path)

        array = np.array(image)

        print(
            f"\nFile: {path.name}"
            f"\n  Size: {image.size}"
            f"\n  Mode: {image.mode}"
            f"\n  Shape: {array.shape}"
            f"\n  Dtype: {array.dtype}"
            f"\n  Min: {array.min()}"
            f"\n  Max: {array.max()}"
            f"\n  Mean: {array.mean():.4f}"
            f"\n  Std: {array.std():.4f}"
        )


def check_pairs():

    degraded_files = {
        p.name
        for p in DEGRADED_DIR.iterdir()
        if p.suffix.lower() in VALID_EXTENSIONS
    }

    gt_files = {
        p.name
        for p in GT_DIR.iterdir()
        if p.suffix.lower() in VALID_EXTENSIONS
    }

    matched = degraded_files & gt_files
    missing_gt = degraded_files - gt_files
    missing_degraded = gt_files - degraded_files

    print("\n========== PAIR CHECK ==========")

    print(
        f"Degraded images: {len(degraded_files)}"
    )

    print(
        f"Ground-truth images: {len(gt_files)}"
    )

    print(
        f"Matched pairs: {len(matched)}"
    )

    print(
        f"Missing ground truth: {len(missing_gt)}"
    )

    print(
        f"Missing degraded: {len(missing_degraded)}"
    )

    if missing_gt:
        print("\nExamples missing GT:")
        for name in list(missing_gt)[:5]:
            print(" ", name)

    if missing_degraded:
        print("\nExamples missing degraded:")
        for name in list(missing_degraded)[:5]:
            print(" ", name)


def main():

    print("================================")
    print("   KLA DATASET INSPECTION")
    print("================================")

    if not DEGRADED_DIR.exists():
        print(
            f"\nERROR: {DEGRADED_DIR} does not exist."
        )
        return

    if not GT_DIR.exists():
        print(
            f"\nERROR: {GT_DIR} does not exist."
        )
        return

    inspect_folder(DEGRADED_DIR)

    inspect_folder(GT_DIR)

    check_pairs()


if __name__ == "__main__":
    main()