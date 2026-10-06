#!/usr/bin/env python3
"""Encode the image as RGB565 and build the licensed UI font subset."""
from __future__ import annotations
import argparse
import hashlib
import json
import subprocess
from pathlib import Path
from PIL import Image
from fontTools import subset
from fontTools.ttLib import TTFont

ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--font", type=Path, default=ROOT / "assets/fonts/CarCard-SansSC.otf")
    parser.add_argument("--converter", default="lv_font_conv")
    args = parser.parse_args()
    text = "".join(path.read_text() for path in (
        ROOT / "main/carcard_ui.c", ROOT / "main/carcard_config.h",
        ROOT / "main/carcard_local_config.h") if path.exists())
    points = sorted(set(range(32, 127)) | {ord(c) for c in text if ord(c) > 127})
    symbols = "".join(map(chr, points))
    fonts = ROOT / "assets/fonts"
    source = TTFont(args.font)
    missing = set(points) - set(source.getBestCmap())
    if missing:
        raise SystemExit("Source font missing: " + ", ".join(f"U+{c:04X}" for c in sorted(missing)))
    sub = subset.Subsetter()
    sub.populate(unicodes=points)
    sub.subset(source)
    for record in source["name"].names:
        if record.nameID in (1, 3, 4, 6):
            name = "CarCardSansSC" if record.nameID == 6 else "CarCard Sans SC"
            record.string = name.encode(record.getEncoding())
    source.save(fonts / "CarCard-SansSC.otf")
    (fonts / "carcard_glyphs.txt").write_text(symbols + "\n")
    (fonts / "carcard_glyphs.h").write_text(
        "#pragma once\n#include <stdint.h>\nstatic const uint32_t carcard_glyphs[] = {\n    "
        + ", ".join(f"0x{c:04X}" for c in points) + "\n};\n")
    for size in (14, 16, 20):
        subprocess.run([args.converter, "--font", str(fonts / "CarCard-SansSC.otf"),
                        "--range", "0x20-0x7E", "--symbols", symbols,
                        "--size", str(size), "--bpp", "4", "--format", "lvgl",
                        "--no-compress", "--lv-include", "lvgl.h", "--lv-font-name",
                        f"carcard_font_{size}", "--output", str(fonts / f"carcard_font_{size}.c")],
                       check=True)
        output = fonts / f"carcard_font_{size}.c"
        output.write_text(output.read_text().rstrip() + "\n")
    images = ROOT / "assets/images"
    image_records = {}
    for name, stem in (("r36", "r36-silver-variant"), ("polo", "polo-silver-9n3")):
        original = images / f"{stem}-source.png"
        # Encoding for the small display; no replacement artwork is synthesized.
        im = Image.open(original).convert("RGB").resize((108, 50), Image.Resampling.NEAREST)
        im = im.resize((216, 100), Image.Resampling.NEAREST)
        im.save(images / f"{stem}.png")
        pixels = bytearray()
        for r, g, b in im.get_flattened_data():
            value = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)
            pixels.extend((value & 255, value >> 8))
        lines = ["    " + ", ".join(f"0x{x:02x}" for x in pixels[i:i+24])
                 for i in range(0, len(pixels), 24)]
        (images / f"carcard_{name}.c").write_text(
            '#include "lvgl.h"\n\nstatic const uint8_t pixels[] = {\n'
            + ",\n".join(lines) + f"\n}};\n\nconst lv_image_dsc_t carcard_{name} = {{\n"
            "    .header = {.magic = LV_IMAGE_HEADER_MAGIC, .cf = LV_COLOR_FORMAT_RGB565,\n"
            "               .w = 216, .h = 100, .stride = 432},\n"
            "    .data_size = sizeof(pixels), .data = pixels,\n};\n")
        image_records[name] = {"flash_bytes": len(pixels),
                               "source_image_sha256": hashlib.sha256(original.read_bytes()).hexdigest()}
    manifest = {"font_converter": "lv_font_conv 1.5.3", "font_glyphs": len(points),
                "sizes": [14, 16, 20], "format": "4 bpp, uncompressed",
                "image_dimensions": [216, 100], "image_format": "RGB565 little-endian",
                "images": image_records}
    (fonts / "carcard_assets.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"CarCard assets: {len(points)} glyphs at 3 sizes; {sum(r['flash_bytes'] for r in image_records.values())} image bytes")


if __name__ == "__main__":
    main()
