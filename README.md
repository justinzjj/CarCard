[简体中文](README.zh_CN.md) · **English**

# CarCard

An offline, pixel-art garage for **FoloToy AI Passport**, based on the full
[FoloToy/ai-passport](https://gitee.com/FoloToy/ai-passport) project.
This application is maintained on `feature/carcard` and reuses the board BSP.

<p align="center">
  <img src="assets/images/r36-silver-variant.png" alt="Silver Volkswagen R36 Variant pixel illustration" width="324">
  <img src="assets/images/polo-silver-9n3.png" alt="Silver Volkswagen Polo hatchback pixel illustration" width="324">
</p>

## Features

- Separate vehicle cards for a silver R36 Variant and a silver 2008 Polo hatchback.
- Pixel illustrations, Chinese text, stock parameters and battery indication.
- Independent, editable mileage saved in NVS and retained across restart.
- Backlight off after 60 seconds without input; any key wakes the screen.
- Public demo plates, with optional private local overrides.

| Control | Vehicle card / parameters | Mileage editor |
| --- | --- | --- |
| UP / DOWN | Switch pages | Increase / decrease selected digit |
| Short OK | Switch vehicles, keeping the page type | Next digit; save on the last digit |
| Hold OK | Edit the selected vehicle's mileage | Cancel changes |

The first gesture after screen-off only wakes the screen. Vehicle data is
manually recorded; the app does not obtain live telemetry from a vehicle.

## Hardware and development

ESP32-C3, 8 MB Flash, no PSRAM, 240 × 320 display and three function buttons.
Use **ESP-IDF 5.5.3**. Read [AGENTS.md](AGENTS.md) before AI-assisted development;
the repository includes five Passport skills under [skills/](skills/README.md).

1. Prepare the toolchain using the [environment guide](docs/development/engineering/environment-setup.md).
2. Activate ESP-IDF with its `export.sh`.
3. Run the complete gate:

```bash
./tools/validate.sh
```

The gate runs host tests, builds firmware, checks the merged image and renders
the actual LVGL pages to verify fonts and text bounds. For focused checks:

```bash
./tools/validate.sh --static
./tools/validate.sh --firmware
```

The verified `build/FoloToy-AI-Passport-full.bin` can provision a device from
**0x0**. Matching ELF/MAP and component images are archived under
`build/firmware/<sha256>/`. Build products and raw logs are not tracked.
Merged flashing may reset NVS; use matching segmented images with a compatible
partition table when mileage must survive. See the [flashing policy](docs/development/engineering/firmware-layout.md#flashing-and-stored-data).

## Private plate configuration

Public builds show `R36 DEMO` and `POLO DEMO`. To personalize a local build,
create `main/carcard_local_config.h`:

```c
#pragma once
#define CARCARD_R36_PLATE "YOUR_R36_PLATE"
#define CARCARD_POLO_PLATE "YOUR_POLO_PLATE"
```

This file is Git-ignored. Defining `CARCARD_PUBLIC_BUILD=1` at compile time
disables the local override for public previews. Custom Chinese characters must
be covered by the generated font subset; follow the [font guide](docs/development/engineering/lvgl-chinese-fonts.md).

## Validation and notes

Firmware build, host tests and LVGL font/layout checks have passed. The owner
confirmed parameter rendering, Chinese text, mileage and page/vehicle controls
on the device. Screen-off/wake behavior, actual power savings and power-loss
persistence still require device acceptance. The Polo engine-family label is
owner-supplied. See the [application guide](docs/apps/carcard.md) for details,
data sources, exact tested firmware identities and remaining checks.

Code is under the [MIT license](LICENSE). The bundled font subset retains its
[SIL Open Font License](assets/fonts/OFL.txt); image provenance and generation
instructions are in the [assets guide](assets/README.md).
