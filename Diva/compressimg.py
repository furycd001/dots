#!/usr/bin/env python3

from PIL import Image
import os
import multiprocessing

def compress_image(file, output_dir, quality=100):
    """Compress a single image and save it to output directory."""
    try:
        filename_without_extension, ext = os.path.splitext(file)
        compressed_image_path = os.path.join(output_dir, f"{filename_without_extension}_compressed{ext}")

        with Image.open(file) as img:
            img.save(compressed_image_path, quality=quality, optimize=True)

        print(f"Compressed: {file} -> {compressed_image_path}")
    except Exception as e:
        print(f"Error compressing {file}: {e}")

def compress_images(output_dir="compressed_images", quality=100):
    """Compress all images in the current directory in parallel."""
    os.makedirs(output_dir, exist_ok=True)

    # Get all image files
    image_extensions = (".jpg", ".jpeg", ".png")
    image_files = [f for f in os.listdir() if f.lower().endswith(image_extensions) and os.path.isfile(f)]

    # Use multiprocessing to speed up compression
    with multiprocessing.Pool(processes=os.cpu_count()) as pool:
        pool.starmap(compress_image, [(file, output_dir, quality) for file in image_files])

    print(f"Compression complete. Compressed images saved to: {output_dir}")

if __name__ == "__main__":
    compress_images()