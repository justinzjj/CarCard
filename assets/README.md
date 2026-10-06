<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Assets

This directory stores reusable fonts, images, music, and sound effects, organized by asset type.

Keep each asset in the matching subdirectory and document its destination, naming, integration method, and source/license. Do not mix binary assets with Markdown documentation.

## Fonts

Store reusable font files and generated font sources in `fonts/`.

CarCard uses `fonts/CarCard-SansSC.otf`, a renamed 179-glyph subset of
[Noto Sans CJK SC Regular](https://github.com/notofonts/noto-cjk/tree/main/Sans),
redistributed under `fonts/OFL.txt`. Uncompressed 4-bpp sources
`carcard_font_14.c`, `carcard_font_16.c`, and `carcard_font_20.c` are compiled by
the application and explicitly bound to labels. The inventory is
`fonts/carcard_glyphs.txt`; startup checks all code points in
`fonts/carcard_glyphs.h` through LVGL. Generate using
`python tools/generate_carcard_assets.py --converter <lv_font_conv-1.5.3>`;
pass the full original font with `--font` when extending the character set.

- Use descriptive names that include the family, weight, size, and format when relevant.
- Document the source, license, character range, conversion command, and expected destination.
- Check Flash and internal-RAM impact before adding a font; the ESP32-C3 has no PSRAM.
- Do not commit fonts whose license does not permit redistribution.

## Images

Store reusable source images and generated display assets in `images/`.

| File | Dimensions and format | Use and source |
| --- | --- | --- |
| `images/r36-silver-variant-source.png` | 1836 × 857, PNG RGB | Silver stock R36 Variant pixel illustration generated with the built-in image tool on 2026-10-06; prompt preserved in `images/r36-prompt.txt`. AI illustration, not a Volkswagen product photograph or accuracy-certified drawing. |
| `images/r36-silver-variant.png` / `images/carcard_r36.c` | 216 × 100, PNG / little-endian RGB565 | CarCard vehicle hero; nearest-neighbor encoding through `tools/generate_carcard_assets.py`, with a 108 × 50 logical grid. The C bitmap occupies 43,200 bytes in Flash. |
| `images/polo-silver-9n3-source.png` | 1843 × 853, PNG RGB | Silver Polo 1.4 manual hatchback with four side doors; generated with the built-in image tool on 2026-10-06. Prompt: `images/polo-prompt.txt`. The owner subsequently confirmed the 2008 year, consistent with the 9N3 illustration; this is not a photograph of the owner's car. |
| `images/polo-silver-9n3.png` / `images/carcard_polo.c` | 216 × 100, PNG / little-endian RGB565 | Polo garage card; same 108 × 50 logical grid and generator as R36, occupying 43,200 bytes in Flash. Each car has its own image descriptor. |
| `images/carcard-community-cover.png` | 1086 × 1448, PNG, portrait 3:4 | Completed community cover generated with the built-in image tool on 2026-10-06 using the two car sprites as references; prompt in `images/carcard-community-cover-prompt.txt`. Copied without conversion and visually inspected at its final upload path. Labeled as an illustration, not a device screenshot; contains no real plates. |
| [`images/home.jpg`](images/home.jpg) | 3840 × 2160, JPEG | Product hero image embedded in both project README files to foreground AI Passport and its open, maker-oriented identity. |
| [`images/readme-hardware-specs.png`](images/readme-hardware-specs.png) | 2172 × 724, PNG RGBA | Optional technical infographic retained as a reference asset; it is no longer used as the homepage hero. Generated for this repository with the built-in image generation tool on 2026-09-17; the six labels and values were checked against the documented hardware contract. |
| [`images/logo-wordmark.png`](images/logo-wordmark.png) | 1648 × 336, PNG RGBA | Transparent black wordmark extracted from the repository's original `images/logo.png`; embedded in both project README files for light backgrounds. |
| [`images/logo-wordmark-dark.png`](images/logo-wordmark-dark.png) | 1648 × 336, PNG RGBA | White version of the extracted wordmark, used by the README `<picture>` element when GitHub is in dark mode. |

- Use descriptive names and document dimensions, pixel format, conversion steps, and destination.
- Prefer formats suitable for the 240 × 320 RGB565 display and account for Flash and internal RAM.
- Preserve editable sources where licensing permits, and record the source and license.
- Never commit device QR secrets, credentials, or personal data in images.

## Music and sound effects

Store reusable music and sound-effect sources in `music/`.

- Document the source, license, sample rate, bit depth, channels, conversion command, and destination.
- Prefer 16 kHz, 16-bit mono PCM when it matches the current BSP audio path.
- Check Flash and internal-RAM cost before embedding audio; stream or chunk long recordings.
- Do not commit media without redistribution permission.
