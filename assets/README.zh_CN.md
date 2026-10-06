<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 资源目录（Assets）

本目录集中存放可复用的资源（字库、图片、音乐等），按资源类型分子目录管理。每个资源放在其类型对应的子目录，并记录放置路径、命名方式、集成方式与来源/许可。二进制资源（字体、图片、音频）不属于纯 markdown 文档，请勿与文档混放。涉及版权/授权的资源需注明来源与许可。

## 字库（fonts）

可复用的字库文件与生成的字库源码放在 `fonts/`。

CarCard 使用 `fonts/CarCard-SansSC.otf`，它是
[Noto Sans CJK SC Regular](https://github.com/notofonts/noto-cjk/tree/main/Sans)
改名后的 179 字形子集，按 `fonts/OFL.txt` 授权再分发。
应用编译并显式绑定到标签的资源为未压缩 4-bpp 的 `carcard_font_14.c`、
`carcard_font_16.c`、`carcard_font_20.c`。字符清单位于 `fonts/carcard_glyphs.txt`，
启动时通过 LVGL 检查 `fonts/carcard_glyphs.h` 中所有码点。
使用 `python tools/generate_carcard_assets.py --converter <lv_font_conv-1.5.3>` 生成；
扩展字符集时通过 `--font` 传入完整原始字体。

- 命名要能反映字族、字重、字级与格式。
- 记录来源、许可、字符范围、转换命令与目标放置路径。
- 添加字库前评估 Flash 与内部 RAM 影响；ESP32-C3 无 PSRAM。
- 不提交许可不允许分发的字库。

## 图片（images）

可复用的源图与生成的显示资产放在 `images/`。

| 文件 | 尺寸与格式 | 用途与来源 |
| --- | --- | --- |
| `images/r36-silver-variant-source.png` | 1836 × 857，PNG RGB | 于 2026-10-06 使用内置图像工具生成的银色原厂 R36 旅行版像素插画，提示词保存在 `images/r36-prompt.txt`。它是 AI 插画，并非大众产品照片或经过精度认证的图纸。 |
| `images/r36-silver-variant.png` / `images/carcard_r36.c` | 216 × 100，PNG / 小端 RGB565 | 车辆名片主图；由 `tools/generate_carcard_assets.py` 以最近邻方式编码，逻辑网格为 108 × 50。C 位图占用 Flash 43,200 字节。 |
| `images/polo-silver-9n3-source.png` | 1843 × 853，PNG RGB | 银色 Polo 1.4 手动四侧门两厢，于 2026-10-06 使用内置图像工具生成。提示词保存在 `images/polo-prompt.txt`。车主后续确认年份为 2008，与 9N3 插画一致；并非车主实车照片。 |
| `images/polo-silver-9n3.png` / `images/carcard_polo.c` | 216 × 100，PNG / 小端 RGB565 | Polo 车库名片，复用 R36 的生成脚本及 108 × 50 逻辑网格，占用 Flash 43,200 字节，各车使用独立图像描述符。 |
| `images/carcard-community-cover.png` | 1086 × 1448，PNG，竖版 3:4 | 2026-10-06 使用内置图像工具、以两辆车的像素图为参考生成的完整社区封面；提示词保存在 `images/carcard-community-cover-prompt.txt`。未转换，复制后在实际上传路径打开检查。标明示意插画，并非实机截图，不含真实车牌。 |
| [`images/home.jpg`](images/home.jpg) | 3840 × 2160，JPEG | 嵌入中英文项目 README 的产品主图，突出 AI Passport 产品形象与开放、人人可创作的理念。 |
| [`images/readme-hardware-specs.png`](images/readme-hardware-specs.png) | 2172 × 724，PNG RGBA | 保留为可选技术参考图，不再用于首页主视觉。于 2026-09-17 使用内置图像生成工具为本仓库生成；已根据文档中的硬件能力契约核对图中的六项标签与参数。 |
| [`images/logo-wordmark.png`](images/logo-wordmark.png) | 1648 × 336，PNG RGBA | 从仓库原始 `images/logo.png` 中精确裁切并去除背景的黑色字标；用于中英文项目 README 的浅色主题。 |
| [`images/logo-wordmark-dark.png`](images/logo-wordmark-dark.png) | 1648 × 336，PNG RGBA | 提取字标的白色版本；README 使用 `<picture>` 在 GitHub 深色主题下显示。 |

- 使用描述性命名，并记录尺寸、像素格式、转换步骤与目标路径。
- 优先采用适合 240 × 320 RGB565 显示的格式，并纳入 Flash 与内部 RAM 考量。
- 许可允许时保留可编辑源文件，并记录来源与许可。
- 图片中不得包含设备二维码秘密、凭证或个人数据。

## 音乐与音效（music）

可复用的音乐与音效源码放在 `music/`。

- 记录来源、许可、采样率、位深、声道、转换命令与目标路径。
- 与当前 BSP 音频路径匹配时优先采用 16 kHz、16 位单声道 PCM。
- 嵌入音频前评估 Flash 与内部 RAM 成本；长录音应流式或分块。
- 无再分发许可不提交媒体文件。
