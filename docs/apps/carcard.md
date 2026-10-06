[简体中文](carcard.zh_CN.md) · **English**

# CarCard: R36 and Polo garage

This checkout implements an offline vehicle card for the owner's silver Volkswagen
Passat R36 Variant (stock, 150,000 km) and silver Volkswagen Polo 1.4 manual
hatchback (four side doors, 120,000 km). These initial mileages were supplied
by the owner. The owner confirms the Polo as a 2008 hatchback with a 1.4 L
inline-four, EA113 engine family, 63 kW (86 PS), 130 N m and a five-speed manual.
The engine family is owner-supplied rather than independently identified from an
engine code. The parameter page displays the year, family, power and gear count;
torque is recorded here, outside the existing six-row parameter screen.
Development branch: `feature/carcard`, based on upstream `33d3d1d`.

## Interface and controls

The independently designed interface uses a dark garage palette, a generated pixel
illustrations of both silver vehicles, Chinese text, a vehicle position indicator,
and a battery indicator. Each vehicle card now includes the owner-supplied license
plate in a blue badge above the illustration. Public source uses `R36 DEMO` and
`POLO DEMO`; private overrides live in Git-ignored `main/carcard_local_config.h`.
The installed, owner-approved firmware retains its personalized plates.
Startup selects R36; the selected vehicle is not persisted.

| Screen | Content | Controls |
| --- | --- | --- |
| Vehicle card | Selected car identity, license plate, pixel illustration, independent total mileage | UP/DOWN switches to specifications; OK cycles cars; hold OK edits the current car |
| Stock specifications | R36 stock figures, or 2008 Polo 1.4 L/EA113 L4/86 PS/63 kW/five-speed manual/front-wheel drive | UP/DOWN returns to the card; OK cycles cars while retaining this page; hold OK edits the current car |
| Mileage editor | Six decimal digits, from 000000 to 999999 km | UP increases and DOWN decreases the selected digit with wraparound; OK advances; OK on the final digit saves; hold OK cancels |

The backlight switches off after 60 seconds without button activity. Any key wakes
it at the original 85% brightness, and the entire wake gesture is consumed without
changing the selected car, page, cursor or unsaved mileage draft. Background battery
updates do not reset the timeout. The app and LCD controller keep running; this is
backlight blanking. The worker checks idle time every 250 ms when otherwise idle.
If button initialization fails, keep the error page lit so it cannot become unwakeable.

Mileage is manually recorded. The firmware does not read live vehicle telemetry.
The initial mileages are not a measured odometer feed.
Stock figures follow Volkswagen's [2008 technical brochure](https://www.volkswagen.se/idhub/content/dam/onehub_pkw/importers/se/hjalp-och-support/broschyrer/passat/before-nbd/000.Passat_Fakta_2008_-_tryck_080625_-_VW729.download.pdf)
and [2008 model information](https://www.volkswagen.es/comunicacion/dossier/salon-del-automovil-de-madrid-gama-volkswagen-2008/).
PS is metric horsepower; the interface does not label it as imperial hp.

The Polo illustration follows the owner's 9N3 identification, consistent with
the confirmed 2008 year and Volkswagen's account of the
[9N3 facelift introduced in 2005](https://www.volkswagen-newsroom.com/en/polo-iv-20012009-19150)
and [multiple 1.4 engine variants](https://www.volkswagen-newsroom.com/de/motorversionen-polo-4-steckbrief-19151).
The header now reads `POLO · 2008 / 9N3`; this identification is based on the
owner's description and model history, not a VIN lookup. The owner confirmed
that the screenshot's power, torque and five-speed manual also apply to the
hatchback. Its sedan body, dimensions, acceleration and maximum speed are not
used. An exact engine code is optional and remains unrecorded. Four side doors
plus the hatch correspond to
the usual five-door body description; the UI retains the owner's four-door wording.
The pixel image is an illustration rather than a photograph of the owner's car.

## Public repository and local configuration

The owner requested publication to [justinzjj/CarCard](https://github.com/justinzjj/CarCard)
and explicitly chose demo plates for public source. `main/carcard_config.h`
provides defaults and optionally loads `main/carcard_local_config.h`; the local
file is ignored by Git and contains the owner's private overrides. Compile with
`CARCARD_PUBLIC_BUILD=1` to ignore those overrides for a public preview.
`tools/generate_carcard_assets.py` includes configuration headers in its glyph
inventory when regenerating fonts. Custom plates still require font coverage
and layout checks. Raw logs, firmware, debug bundles and toolchains remain local.
Publication does not flash or change the installed firmware. The following
device results identify the personalized builds tested before publication.

Public-source validation on 2026-10-06 used an isolated staged-source snapshot
without the ignored local header. Build and host tests passed, including actual
demo-plate rendering, font/text bounds and 300 transitions. The public merged
image and manifest were verified, and the image contains neither private plate.
Private-override isolation and the local personalized preview also passed.
No new device flash was requested or performed for publication; on-device checks
of the demo-plate variant are NOT RUN. Local logs and previews remain under
`build/carcard-public-validation.log` and `build/carcard-public-preview.png`.

```text
Public snapshot full image SHA-256: ca798f17b725b4ec61014f116295c1498ac1c698b9c8c82cf375d0a0741a9f85
Public snapshot matching ELF SHA-256: 838f622e39a4ee6b26f217dc78979b1bc4ec4202bad224f300a63f7ff9794094
```

The 963,920-byte public snapshot image is archived locally under
`build/carcard-public-source/build/firmware/<full-image-sha256>/` and can be
flashed from `0x0` under the data policy. It is not uploaded with the source.

## Implementation

- `main/carcard_model.c`: hardware-independent vehicle selection, page navigation, per-car digit editing, cancellation,
  and save acknowledgement. Editing locks the selected vehicle; a save failure
  leaves its draft editable for retry.
- `main/carcard_idle.c`: pure monotonic 60-second deadline and wake transitions.
  Boundary/activity-reset tests also cover long-running clocks.
- `main/carcard_input.c`: BSP-event translation and wake-gesture suppression.
  Delayed OK click/double/long events are consumed, and the gesture END marker
  clears suppression even after three or more taps. UP/DOWN use PRESS, avoiding delayed
  or lost rapid taps; OK uses CLICK/DOUBLE/LONG to preserve long-press editing and
  cancellation. A double click produces two logical OK actions.
- `main/carcard_store.c`: NVS loading and committing; host fault tests cover missing,
  corrupt and zero records, read/write/commit failures, independent restart
  restoration and compatibility with the earlier R36-only installation.
- `main/carcard_ui.c`: the actual LVGL screens, explicit fonts on every label, and
  startup glyph checks. The application does not compile the demo visual shell.
- `main/main.c`: BSP initialization, a bounded button queue, application worker,
  battery polling, and storage coordination. Callbacks enqueue only; LVGL access holds the BSP lock.
- BSP display/battery drivers, ADC pins/thresholds and the minimal 8 MB factory
  partition layout are reused. The button interface additionally forwards the
  component's gesture-end event as appended `BSP_BTN_END`; existing event values
  and the original four callbacks remain unchanged.
  Wi-Fi/BLE and audio playback are not initialized by the application.
- Storage uses the NVS namespace `carcard` and unsigned 32-bit keys `mileage_km`
  for R36 (unchanged) and `polo_km` for Polo. Loading does not rewrite old records;
  an absent Polo record uses 120,000 km. A bad record only flags the affected car.
  Only a successful `nvs_set_u32` and `nvs_commit` updates the displayed saved value.
  NVS setters can write before commit; a reported commit failure is not a rollback
  guarantee, and a restart may load that newly written value.
  Zero is valid. Missing records use the supplied default. Invalid records show a
  warning. Initialization failures never erase NVS automatically.

## Build and acceptance

Use ESP-IDF **5.5.3** and the shared gate:

```sh
./tools/validate.sh --static
./tools/validate.sh
```

This session's isolated installation is under `build/tooling/`. To reactivate it:

```sh
export IDF_TOOLS_PATH="$PWD/build/tooling/idf-tools"
export PATH="$PWD/build/tooling/host-venv/bin:$PATH"
source build/tooling/esp-idf-v5.5.3/export.sh
idf.py --version
```

Generated assets can be rebuilt with Pillow, fontTools, and `lv_font_conv@1.5.3`:

```sh
python tools/generate_carcard_assets.py --converter build/tooling/font-converter/node_modules/.bin/lv_font_conv
```

New Chinese strings may require the full upstream font passed with `--font`.
The subset supports the recorded inventory, not arbitrary Chinese text.
Asset sources, licensing, and the image-generation prompt are in [Assets](../../assets/README.md).

The complete gate also renders the actual application UI on a native LVGL
display. It checks every label's selected font and bounds, a deliberately missing
glyph, failure messages, both cars and editors, and 300 page/vehicle transitions. Images are written as PPM
under `build/carcard-preview/`; this is a host preview, not a device screenshot.
The 24 KB LVGL pool test uses a 40-line RGB565 draw buffer outside that pool,
matching the BSP's partial-rendering approach.

Before hardware acceptance, check cold startup, both cars and screens, mixed Chinese/Latin
text, three-button response, all six editing positions, cancellation, saving and
independent restart persistence, legacy R36 upgrade, unavailable battery indication, and memory during repeated
page switching. A host render or a successful firmware build does not validate
screen colors, ADC timing, physical buttons, battery calibration or real Flash.

For the inactivity feature, test initial idle blanking, resetting the deadline with
an action near 60 seconds, waking with each button and single/double/triple/long OK
gestures, a normal action immediately after waking, and preserving an editor draft.
Battery refresh must not wake the screen. Actual backlight timing and wake gestures
require the new build on the board.

## Current delivery: confirmed Polo parameters, 2026-10-06

The Polo page now shows `POLO · 2008 / 9N3`, `EA113 L4`, `86 PS`, `63 kW`
and a five-speed manual. The owner confirmed these powertrain figures for the
hatchback; the screenshot's sedan dimensions and performance are not adopted.
No additional owner data is required for the current parameter page. An exact
engine code can optionally refine the owner-supplied engine family later.

- Build: **PASS**, ESP-IDF v5.5.3 and the complete shared validation gate.
- Host tests: **PASS**, existing input/idle/storage tests, selected-font glyph
  coverage, text bounds and 300 page/vehicle transitions. The updated Polo
  parameter rendering was visually inspected; the LVGL 24 KB pool retained
  6,816 bytes free with a 6,360-byte largest block and 17,800-byte peak use.
- Device tests: **PASS for the observed and owner-confirmed checks**.
  On 2026-10-06 the owner approved this exact parameter build and a segmented
  update retaining mileage storage. The same ESP32-C3 at `/dev/cu.usbmodem1101`
  received the archived bootloader at `0x0`, partition table at `0x8000` and
  app at `0x10000`. All three write hashes passed, with no NVS/PHY sector writes.
  The 25-second startup capture matched ELF prefix `fdfc1b0c6`, reported fonts/
  storage/buttons/battery ready and 236,760 bytes free heap, and showed no runtime
  crash. USB resets during monitor attachment were operator-triggered. The port
  is closed. The owner confirmed the updated parameter page, Chinese rendering,
  both mileage readings and UP/DOWN page navigation plus OK vehicle selection
  all work normally.
- Unverified: 60-second screen-off/wake acceptance, actual power savings and
  power-loss mileage persistence.
  The engine family has not been independently checked against an engine code.

Verified merged image:
`build/firmware/eb8d511954791f5d28d0059ab15647906e6a9d80a60f68ad02de3c6b73fb9cb1/FoloToy-AI-Passport-full.bin`,
**963,920 bytes**, flash offset **0x0**. Matching ELF/MAP, component images and
manifest are retained in that archive.

```text
Full image SHA-256: eb8d511954791f5d28d0059ab15647906e6a9d80a60f68ad02de3c6b73fb9cb1
Matching ELF SHA-256: fdfc1b0c6f6ee60754e57e7ca0fff1f8e782c7b097c44b36a44b52a8ed507e38
```

Validation log: `build/carcard-polo-specs-validation.log`; host preview:
`build/carcard-polo-specs-preview.png`. Application work is maintained on
`feature/carcard`. This exact build was flashed with the owner's approval using
archived components and the unchanged partition table to retain NVS. Local raw
logs are `build/carcard-polo-specs-flash.log` and
`build/carcard-polo-specs-startup.log`; do not publish them unsanitized.
A complete merged refresh can reset NVS.

## Previous delivery: revised navigation, 2026-10-06

On ordinary screens, UP/DOWN switches between the card and specifications;
OK cycles vehicles while retaining the current page type. The on-screen hint
matches this mapping. Mileage-editor controls and the 60-second inactivity
feature remain as documented above.

- Build: **PASS**, ESP-IDF v5.5.3, complete shared gate and merged-archive verification.
- Host tests: **PASS**, both directions of page navigation, cyclic OK vehicle
  selection on both screens, unchanged editor/save/cancel behavior, BSP input
  mapping, wake-gesture isolation with the revised controls, persistence contracts,
  actual label font/bounds checks and 300 page/vehicle transitions.
  The updated two-card host preview was visually inspected.
- Device tests: **PASS for flashing and startup; behavior acceptance pending**.
  On 2026-10-06 the owner explicitly authorized both completed features. The
  verified archive was flashed to the same ESP32-C3 using matching component
  images: bootloader at `0x0`, partition table at `0x8000`, app at `0x10000`.
  All three write hashes passed. The unchanged partition table permits this
  segmented update without writing the NVS or PHY data sectors. A 25-second
  startup capture matched ELF prefix `5134afe19`, reported fonts/storage/buttons/
  battery ready and 236,760 bytes free heap, and showed no runtime crash.
- Unverified: revised physical controls and visible 60-second timeout/wake
  behavior, actual power savings, power-loss persistence, battery accuracy,
  runtime heap/stack margins and pending Polo specifications.

Merged firmware: `build/firmware/ad96c5007d369b67c8d55063bc3041f5eb9e2d2e2029e29c88afd5fbf7ded5f5/FoloToy-AI-Passport-full.bin`,
**963,904 bytes**, offset **0x0**. Matching ELF/MAP and manifest are retained
in the verified archive.

```text
Full image SHA-256: ad96c5007d369b67c8d55063bc3041f5eb9e2d2e2029e29c88afd5fbf7ded5f5
Matching ELF SHA-256: 5134afe191d0baf16409d5aff709e9fa62076d8b79591d8125f9588572fc464b
```

Validation log: `build/carcard-controls-validation.log`; current preview:
`build/carcard-controls-preview.png`. Changes remain uncommitted on
`feature/carcard`. This exact build was flashed with the owner's approval using
the archived components to retain NVS. Local raw logs are
`build/carcard-controls-flash.log` and `build/carcard-controls-startup.log`;
an additional 45-second capture is in `build/carcard-controls-runtime.log`.
Opening the native USB monitor triggered a USB reset, so this second window
also contains startup rather than a continuous 60-second inactivity observation.
The serial port has been closed. Do not publish these logs unsanitized.
On-device navigation and timeout/wake acceptance
has been requested from the owner. The merged image remains available for a
complete refresh from `0x0`, which can reset NVS.

## Previous delivery: 60-second screen blanking, 2026-10-06

Added monotonic inactivity timing in the app worker, with backlight off at the
60-second deadline and restoration to 85% on any key. A wake gesture does not
change the model, including an unsaved editor draft. Battery refresh remains
independent. Raw BSP events are queued without I/O in the callback; the existing
button component now forwards gesture END so triple-tap wake cannot swallow the
next independent OK gesture. Input initialization failure keeps the screen lit.

- Build: **PASS**, ESP-IDF v5.5.3, complete shared gate and verified merged archive.
- Host tests: **PASS**, exact 60-second boundary, one-time off transitions, activity
  reset, large monotonic clocks, UP/DOWN/OK wake consumption, delayed single/double/long
  OK events, triple-tap followed by independent click/hold, and draft preservation.
  BSP fault tests cover all 15 callback registration failures, including END;
  the existing model/input/storage and native LVGL checks also pass.
  Independent code review found no remaining actionable issues.
- Device tests: **NOT RUN for this build**. The ESP32-C3 is connected at
  `/dev/cu.usbmodem1101` but still runs the previously authorized plate version.
  No newer image was written without its own authorization.
- Unverified: physical 60-second timing, backlight off/wake behavior, wake gesture
  isolation, draft retention on device, actual power savings, power-loss mileage
  persistence, battery accuracy, runtime heap/stacks and the pending Polo specifications.

Merged firmware: `build/firmware/b00454b2adc1c32e2ca79d9b24c50452239c3a49f84163276d66229e8cb64df8/FoloToy-AI-Passport-full.bin`,
**963,904 bytes**, offset **0x0**, with matching ELF/MAP and verified manifest.

```text
Full image SHA-256: b00454b2adc1c32e2ca79d9b24c50452239c3a49f84163276d66229e8cb64df8
Matching ELF SHA-256: 018a53bfd0b1ab638da5f82867267be0c951e49029f5d8ca9f2cc97cca2468d2
```

Complete validation log: `build/carcard-idle-validation.log`. Work remains
uncommitted on `feature/carcard`. This new image requires explicit flash approval;
merged flashing can reset NVS. The unchanged partition table and matching component
images allow compatible segmented flashing when preserving records is requested.

## Previous delivery: license plates, 2026-10-06

Both vehicle cards show their corresponding owner-supplied plate in a blue badge.
The pixel image was moved down to reserve a separate row; mileage and controls
are unchanged. The font inventory now contains 179 glyphs at all three sizes.

- Build: **PASS**, ESP-IDF v5.5.3, complete shared gate, merged-image and archive verification.
- Host tests: **PASS**, repository/model/input/storage checks, actual LVGL active
  glyph and text-bound checks, ten rendered states and 300 page/vehicle transitions.
  The host preview was visually inspected for both plate-to-car assignments.
- Device tests: **PASS for the observed checks**. On 2026-10-06 the owner authorized
  flashing the exact plate-version archive above. A single ESP32-C3 with embedded
  8 MB Flash was identified at `/dev/cu.usbmodem1101`. Esptool v4.12.0 wrote the
  963,408-byte merged image at `0x0` and verified its data hash. The written-sector
  range was `0x00000000` through `0x000EBFFF`; no whole-chip erase was used.
  A bounded 25-second startup observation showed one application start and Ready
  event, with no panic/watchdog/crash marker. The reported ELF hash prefix matches
  the archived ELF. Fonts, storage, buttons and battery initialized successfully;
  startup free heap was 237,008 bytes. The owner confirmed both cars' plates,
  Chinese text and pixel pictures, vehicle/page switching, and entering/cancelling
  mileage editing all displayed and worked normally. The serial port was closed.
- Unverified: save-and-power-cycle persistence, battery accuracy, heap behavior
  during prolonged use, task stack margins, long-duration stability, and the
  previously pending Polo specifications.
- Native LVGL 24 KB pool: peak 17,800 bytes, settled free 6,816 bytes,
  largest free block 6,360 bytes; this is not device heap measurement.

Merged firmware: `build/firmware/179810e5a8280ae00795125f7c7ec0074e22949620ada3502032749856fc7b7f/FoloToy-AI-Passport-full.bin`,
**963,408 bytes**, offset **0x0**. Matching ELF/MAP and the verified manifest
are retained in the same bundle.

```text
Full image SHA-256: 179810e5a8280ae00795125f7c7ec0074e22949620ada3502032749856fc7b7f
Matching ELF SHA-256: 211bb70ae965285e9d801178e2ab247b9276dc025a564f974eb2620a22dffe40
```

Validation log: `build/carcard-plates-validation.log`; host preview:
`build/carcard-plates-preview.png`. Work remains local and uncommitted on
`feature/carcard`. This exact merged image was flashed with explicit approval;
NVS was within the merged write range. Local raw flash/startup logs are retained
under `build/carcard-plates-flash.log` and `build/carcard-plates-startup.log`;
these logs are not intended for publication.

## Previous delivery: two-car garage without plates, 2026-10-06

- Build: **PASS**, ESP-IDF v5.5.3, complete shared gate, merged layout and archive verification.
- Host tests: **PASS**, repository/baseline checks, two-car navigation and editing,
  BSP event translation, independent NVS records and legacy R36 compatibility,
  error/restart contracts, all 177 glyphs at three sizes, active-label font and
  bounds checks, ten rendered states and 300 transitions across all four normal screens.
- Device tests: **NOT RUN**. No Espressif USB candidate was detected in the
  current macOS USB inventory; no serial port was opened and no firmware flashed.
- Unverified: real display/color/Chinese rendering, physical buttons, battery
  readings, actual NVS persistence/power loss, runtime heap/stacks, and Polo year,
  generation and precise engine/power. The image follows the tentative 9N3 identity.
- Native LVGL 24 KB pool: peak allocation 17,808 bytes, settled free 7,272 bytes,
  largest free block 6,840 bytes. This is a host pool measurement, not device heap.
- Firmware MAP: static D/IRAM 104,190 bytes (217,106 bytes remain), Flash sections
  822,210 bytes; application image 896,944 bytes in the unchanged 8 MB layout.

Merged firmware: `build/firmware/4f1dbd84e38fb7415b3f089fb17956afd24db36cbe16666d5c23cd2ecb3050d6/FoloToy-AI-Passport-full.bin`,
**962,480 bytes**, flash offset **0x0**. The bundle also retains the matching
ELF/MAP, app/bootloader/partition images, flash arguments and verified manifest.

```text
Full image SHA-256: 4f1dbd84e38fb7415b3f089fb17956afd24db36cbe16666d5c23cd2ecb3050d6
Matching ELF SHA-256: e478071efbe90401f7245b0432c46d20e4cbdbc0f9d0063a5f1e4c94fa4be66a
```

Validation log: `build/carcard-garage-validation.log`. Current host preview:
`build/carcard-garage-preview.png`. Work remains uncommitted on `feature/carcard`;
no commit, publishing or flashing was authorized. The merged image may reset NVS;
compatible segmented flashing can preserve the existing R36 record under the
[flashing policy](../development/engineering/firmware-layout.md#flashing-and-stored-data).

## Previous delivery: R36-only, 2026-10-06

- Build: **PASS**, ESP-IDF v5.5.3, shared firmware gate and merged-image verification.
- Host tests: **PASS**, repository checks, baseline fault tests, CarCard model,
  BSP-event translation, NVS error/restart contracts, actual LVGL glyph/bounds checks,
  seven rendered states and 300 page transitions. NVS fault stubs reflect setters
  writing immediately; commit failures are checked as errors, not rollback guarantees.
- Device tests: **NOT RUN**. No Espressif USB candidate was detected; no ports were
  opened and no firmware was flashed.
- Unverified: real display/color/Chinese rendering, physical button timing, actual
  NVS behavior during power loss/restart, battery readings, runtime heap and stacks.
- Native LVGL pool: 24 KB, peak allocation 17,816 bytes, settled free 3,592 bytes,
  largest free block 2,984 bytes. Native pointers differ from ESP32 pointers; this
  is not a measurement of device heap.

Historical merged image in the archive below, **915,168 bytes**, offset **0x0**.
The matching ELF/MAP and manifest are retained under
`build/firmware/4be8b5cb0116913ff7fca28af42066da5353b3cc879b32b3609ef8e561fe3728/`.

```text
Full image SHA-256: 4be8b5cb0116913ff7fca28af42066da5353b3cc879b32b3609ef8e561fe3728
Matching ELF SHA-256: c6f45978b72815e0ac3d6badb13d5f43d6837902db96f94acc065d64dbb076d2
```

Changes remain uncommitted on `feature/carcard`. Neither a commit nor publishing
was requested. The UI preview is `build/carcard-ui-preview.png`.

The delivered merged image belongs at **0x0**. Flashing it may reset NVS, including
previous settings and edited mileage. [Flashing policy](../development/engineering/firmware-layout.md#flashing-and-stored-data)
applies; flashing requires the owner's explicit approval for the exact image.
