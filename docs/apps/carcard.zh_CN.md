[English](carcard.md) · **简体中文**

# CarCard：R36 与 Polo 车库

本工作区实现离线车库，包括车主的银色大众帕萨特 R36 旅行版（原厂，150,000 km）
和银色大众 Polo 1.4 手动两厢（四个侧门，120,000 km）。初始里程来自车主，
车主确认 Polo 为 2008 年两厢，1.4 L 直列四缸、EA113 系列、63 kW（86 PS）、
130 N·m、5 挡手动。发动机系列由车主提供，未通过具体发动机代号独立核验。
参数页显示年份、发动机系列、功率、马力及挡位数量；扭矩记录在本文，
未增加到现有六行参数页。开发分支为 `feature/carcard`，
基于上游 `33d3d1d`。

## 界面与操作

界面独立设计，采用深色车库配色、两辆银色车的像素插画、中文文字、车辆序号和电量显示。
两辆车的名片均在像素图上方显示车牌，使用蓝色牌框。公开源码使用 `R36 DEMO` 和
`POLO DEMO`；私有覆盖位于 Git 忽略的 `main/carcard_local_config.h`。
设备上已授权刷入的固件保留个性化车牌。
开机默认选择 R36，当前选车不持久化。

| 页面 | 内容 | 操作 |
| --- | --- | --- |
| 车辆名片 | 当前车型、车牌、像素插画、独立总里程 | 上下键切换参数页；确认循环换车；长按确认编辑当前车里程 |
| 原厂参数 | R36 原厂数据，或 2008 年 Polo 1.4 L、EA113 L4、86 PS、63 kW、5 速手动、前驱 | 上下键返回名片；确认换车并保持参数页；长按确认编辑当前车里程 |
| 里程编辑 | 六位数字，范围 000000–999999 km | 上键增加、下键减少当前位，数字循环；确认切换下一位，最后一位确认保存；长按确认取消 |

60 秒无按键操作后关闭背光，任意键唤醒并恢复原来的 85% 亮度。
唤醒的整次按键手势被消耗，不改变选车、页面、光标或未保存的里程草稿。
后台电量更新不重置计时；应用和 LCD 控制器继续运行，此功能通过关闭背光熄屏。
工作任务空闲时每 250 ms 检查计时；如果按键初始化失败，错误页面保持亮屏，避免无法唤醒。

里程为手动记录，固件不读取实时车辆数据。初始里程来自车主提供，
不是通过车辆仪表采集。原厂参数依据大众官方
[2008 年技术手册](https://www.volkswagen.se/idhub/content/dam/onehub_pkw/importers/se/hjalp-och-support/broschyrer/passat/before-nbd/000.Passat_Fakta_2008_-_tryck_080625_-_VW729.download.pdf)
和[2008 年车型资料](https://www.volkswagen.es/comunicacion/dossier/salon-del-automovil-de-madrid-gama-volkswagen-2008/)。
PS 表示公制马力，界面不将其标为英制 hp。

Polo 插画按车主提供的 9N3 代号绘制，与已确认的 2008 年份及大众车型历史一致。
大众官方资料记载
[9N3 于 2005 年改款](https://www.volkswagen-newsroom.com/en/polo-iv-20012009-19150)，
且[存在多种 1.4 发动机版本](https://www.volkswagen-newsroom.com/de/motorversionen-polo-4-steckbrief-19151)。
标题现显示 `POLO · 2008 / 9N3`；此识别依据车主描述和车型历史，并非 VIN 查询。
车主确认截图中的动力参数、扭矩及 5 挡手动同样适用于自己的两厢版。
截图中的三厢车身、尺寸、加速及最高车速不采用。具体发动机代号可选补充，尚未记录。
四个侧门加尾门通常称为五门两厢，界面沿用车主的“四门”描述。
像素图为车型插画，并非车主实车照片。

## 公开仓库与本地配置

车主要求上传至 [justinzjj/CarCard](https://github.com/justinzjj/CarCard)，
并明确选择公开源码使用示例车牌。`main/carcard_config.h` 提供默认值，
可选载入 Git 忽略的 `main/carcard_local_config.h`，后者保存车主的私有覆盖。
编译时定义 `CARCARD_PUBLIC_BUILD=1` 可忽略这些覆盖，生成公开预览。
重新生成字体时，`tools/generate_carcard_assets.py` 会将配置头文件加入字形清单。
自定义车牌仍需检查字形覆盖和布局。原始日志、固件、调试归档及工具链保留在本地。
上传不烧录、不改变设备当前固件；下文实机结果对应公开上传前测试的个性化固件。

2026-10-06 的公开源码验证使用暂存源码的隔离快照，不包含被忽略的本地头文件。
Build 和 Host tests 均通过，包括真实 LVGL 示例车牌渲染、字体/文字边界及
300 次切换。公开合并镜像及清单校验通过，镜像不含任何真实车牌。
私有覆盖隔离检查及本地个性化预览也通过。本次上传未要求或执行新烧录，
示例车牌版本的 Device tests 为 NOT RUN。本地日志和预览分别为
`build/carcard-public-validation.log` 和 `build/carcard-public-preview.png`。

```text
Public snapshot full image SHA-256: ca798f17b725b4ec61014f116295c1498ac1c698b9c8c82cf375d0a0741a9f85
Public snapshot matching ELF SHA-256: 838f622e39a4ee6b26f217dc78979b1bc4ec4202bad224f300a63f7ff9794094
```

公开源码快照镜像为 963,920 字节，本地归档位于
`build/carcard-public-source/build/firmware/<full-image-sha256>/`，
可按数据政策从 `0x0` 刷写；不随源码上传。

## 实现

- `main/carcard_model.c`：独立于硬件的选车、翻页、各车逐位编辑、取消及保存确认逻辑。
  编辑期间锁定当前车，保存失败保留该车草稿，允许重试。
- `main/carcard_idle.c`：纯逻辑的单调时钟 60 秒计时和唤醒转换，测试包含边界、
  活动重置及长时间运行的时钟。
- `main/carcard_input.c`：BSP 事件转换和唤醒手势消耗。确认键后续单击、双击和长按被消耗，
  手势结束事件使三连按及更多连按后也能清除消耗状态。
  上、下键使用 PRESS，避免快速连按延迟或丢失；
  确认键使用 CLICK/DOUBLE/LONG，保留长按编辑与取消。双击产生两次逻辑确认操作。
- `main/carcard_store.c`：NVS 读取与提交；主机故障测试覆盖缺失、损坏及零里程记录、
  读写和提交失败、各车独立模拟重启恢复，以及旧 R36 单车版本兼容。
- `main/carcard_ui.c`：实际 LVGL 页面，每个标签显式选择字体，启动时检查字形。
  应用不编译 demo 视觉框架。
- `main/main.c`：BSP 初始化、有界按键队列、应用工作任务、电量轮询及存储协调。
  按键回调仅入队，操作 LVGL 时持有 BSP 锁。
- 复用 BSP 显示、电量驱动、ADC 引脚及门限和最小 8 MB factory 分区。
  按键接口新增 `BSP_BTN_END` 转发组件手势结束事件，原有事件数值及四个回调保持不变。
  应用不初始化 Wi-Fi/BLE 或音频播放。
- NVS 命名空间为 `carcard`，32 位无符号键为 R36 的 `mileage_km`（保留原键）
  和 Polo 的 `polo_km`。读取不改写旧记录；Polo 无记录时使用 120,000 km，
  某一辆记录异常仅提示该车。
  仅 `nvs_set_u32` 和 `nvs_commit` 都成功后才更新已保存的显示值。
  NVS setter 可能在 commit 前就写入；提交报错不代表数据回滚，重启可能读到新写入的值。
  零里程合法，无记录时使用给定初始值，无效记录显示警告，初始化失败不会自动擦除 NVS。

## 构建与验收

使用 ESP-IDF **5.5.3** 和统一验证入口：

```sh
./tools/validate.sh --static
./tools/validate.sh
```

本次独立安装位于 `build/tooling/`，重新激活方式：

```sh
export IDF_TOOLS_PATH="$PWD/build/tooling/idf-tools"
export PATH="$PWD/build/tooling/host-venv/bin:$PATH"
source build/tooling/esp-idf-v5.5.3/export.sh
idf.py --version
```

使用 Pillow、fontTools 和 `lv_font_conv@1.5.3` 可重新生成资源：

```sh
python tools/generate_carcard_assets.py --converter build/tooling/font-converter/node_modules/.bin/lv_font_conv
```

新增中文文字可能需要通过 `--font` 指定完整的上游字体。
现有子集仅支持记录的字符清单，不支持任意中文。
资源来源、授权和图像生成提示词见[资源说明](../../assets/README.zh_CN.md)。

完整验证入口还会用原生 LVGL 显示器渲染实际应用界面，检查每个标签当前选择的字体及边界、
刻意缺失的字形、错误提示、两车名片及编辑器和 300 次页面/车辆切换。PPM 图片写入 `build/carcard-preview/`，
这是主机预览，不是设备截图。24 KB LVGL 内存池测试使用位于池外的 40 行 RGB565 绘图缓冲，
与 BSP 的局部渲染方式一致。

硬件验收需检查冷启动、两辆车及各页显示、中英文混排、三键响应、六位编辑、取消、各车保存后独立重启恢复、旧 R36 升级兼容、
电量不可用提示和反复翻页时的内存。主机渲染及固件构建成功不能证明屏幕颜色、ADC 时序、
实体按键、电量计精度和实际 Flash 行为。

熄屏功能需实机检查开机无操作熄屏、接近 60 秒时操作重置计时、三个按键唤醒、
确认键单击/双击/三连按/长按唤醒、唤醒后下一次正常操作和编辑草稿保留。
电量刷新不得唤醒屏幕。实际背光时序及唤醒手势需将新版刷入设备后验收。

## 当前交付：补齐 Polo 参数，2026-10-06

Polo 参数页现显示 `POLO · 2008 / 9N3`、`EA113 L4`、`86 PS`、`63 kW`
及 5 速手动。车主确认这些动力参数适用于自己的两厢版；截图中的三厢尺寸
和性能数据不采用。完成当前参数页不需要车主提供更多信息，后续可选补充
具体发动机代号，进一步核对车主提供的发动机系列。

- Build：**PASS**，ESP-IDF v5.5.3，完整统一验证通过。
- Host tests：**PASS**，原有按键、熄屏、存储测试，实际字体字形覆盖、文字边界及
  300 次车辆/页面切换通过。已目视检查更新后的 Polo 参数页渲染；LVGL 24 KB
  内存池空闲 6,816 字节、最大连续块 6,360 字节、峰值使用 17,800 字节。
- Device tests：**已观察及车主确认项 PASS**。
  2026-10-06 车主授权刷入本次准确参数版，采用分段更新保留里程存储。
  向 `/dev/cu.usbmodem1101` 的同一台 ESP32-C3 写入归档中的引导程序 `0x0`、
  分区表 `0x8000` 和应用 `0x10000`。三段写入哈希均通过，未写入 NVS/PHY 扇区。
  25 秒启动日志匹配 ELF 前缀 `fdfc1b0c6`，字体、存储、按键、电量计均初始化成功，
  空闲堆 236,760 字节，未见运行崩溃。串口连接时的 USB 重启由监视操作触发。
  串口已关闭。车主确认新参数页、中文显示、两辆车的里程、上下切页及确认换车
  均正常。
- Unverified：60 秒熄屏/唤醒行为验收、实际省电幅度及断电里程恢复。
  发动机系列尚未通过具体发动机代号独立核验。

已验证的合并固件：
`build/firmware/eb8d511954791f5d28d0059ab15647906e6a9d80a60f68ad02de3c6b73fb9cb1/FoloToy-AI-Passport-full.bin`，
**963,920 字节**，刷写地址 **0x0**；归档保留匹配的 ELF、MAP、分段镜像及清单。

```text
Full image SHA-256: eb8d511954791f5d28d0059ab15647906e6a9d80a60f68ad02de3c6b73fb9cb1
Matching ELF SHA-256: fdfc1b0c6f6ee60754e57e7ca0fff1f8e782c7b097c44b36a44b52a8ed507e38
```

验证日志为 `build/carcard-polo-specs-validation.log`，主机预览为
`build/carcard-polo-specs-preview.png`。应用维护在 `feature/carcard`。
本次准确固件已获车主授权并烧录，使用归档中匹配的分段镜像及未改变的分区表保留 NVS。
本地原始日志为 `build/carcard-polo-specs-flash.log` 和
`build/carcard-polo-specs-startup.log`，未经脱敏不得发布。
完整合并刷新可能重置 NVS。

## 上次交付：调整导航按键，2026-10-06

普通页面中，上下键在名片和参数页之间切换，确认键循环换车并保持当前页面类型。
屏幕提示同步更新；里程编辑及 60 秒无操作熄屏行为保持上述说明。

- Build：**PASS**，ESP-IDF v5.5.3，完整统一验证及合并固件归档校验通过。
- Host tests：**PASS**，上下双向切页、两种页面上确认循环换车、原有编辑/保存/取消、
  BSP 输入映射、新按键逻辑下的唤醒消耗、存储契约、实际标签字体和文字边界及
  300 次页面/车辆切换通过，已目视检查更新后的两车名片预览。
- Device tests：**烧录和启动已观察项 PASS，行为验收待确认**。
  2026-10-06 车主明确授权刷入两个已完成功能。使用已验证归档中的匹配镜像，
  向同一台 ESP32-C3 分段写入：引导程序 `0x0`、分区表 `0x8000`、应用 `0x10000`，
  三段写入哈希校验均通过。新旧分区表一致，本次更新未写入 NVS 或 PHY 数据扇区。
  25 秒启动日志匹配 ELF 前缀 `5134afe19`，字体、存储、按键、电量计均初始化成功，
  空闲堆内存 236,760 字节，观察窗口内未见运行崩溃。
- Unverified：实机新按键操作、屏幕实际 60 秒熄屏与唤醒表现、实际省电幅度、
  断电里程恢复、电量精度、堆内存及任务栈余量，以及待确认的 Polo 参数。

合并固件：`build/firmware/ad96c5007d369b67c8d55063bc3041f5eb9e2d2e2029e29c88afd5fbf7ded5f5/FoloToy-AI-Passport-full.bin`，
**963,904 字节**，刷写地址 **0x0**；已验证归档保留匹配 ELF、MAP 和清单。

```text
Full image SHA-256: ad96c5007d369b67c8d55063bc3041f5eb9e2d2e2029e29c88afd5fbf7ded5f5
Matching ELF SHA-256: 5134afe191d0baf16409d5aff709e9fa62076d8b79591d8125f9588572fc464b
```

验证日志为 `build/carcard-controls-validation.log`，当前预览为
`build/carcard-controls-preview.png`，修改保留在 `feature/carcard`，未提交。
本版已获车主授权烧录，使用归档中匹配的分段镜像保留 NVS。
本地原始日志为 `build/carcard-controls-flash.log` 和
`build/carcard-controls-startup.log`；额外 45 秒记录为
`build/carcard-controls-runtime.log`。打开原生 USB 串口监视触发了 USB 重启，
因此后一个窗口也包含启动过程，未形成连续 60 秒无操作观察。串口现已关闭，
这些原始日志未经脱敏不得发布。
已请车主验收新导航及熄屏/唤醒表现。合并固件仍可用于从 `0x0` 完整刷新，
此方式可能重置 NVS。

## 上次交付：60 秒自动熄屏，2026-10-06

应用工作任务使用单调时钟计时，60 秒无操作时关闭背光，任意键唤醒并恢复 85% 亮度。
唤醒手势不改变状态模型，包括尚未保存的里程草稿；电量更新独立运行。
回调仅将 BSP 原始事件入队，不进行 I/O；现有按键组件额外转发手势 END，
使三连按唤醒不会吞掉下一次独立的确认操作。按键初始化失败时保持亮屏。

- Build：**PASS**，ESP-IDF v5.5.3，完整统一验证和合并固件归档校验通过。
- Host tests：**PASS**，精确 60 秒边界、仅一次熄屏转换、活动重置、大数单调时钟、
  上下确认键唤醒消耗、后续单击/双击/长按、三连按唤醒后的独立单击与长按、草稿保留均通过。
  BSP 故障测试覆盖包含 END 的全部 15 个回调注册失败点；原有模型、输入、存储及原生 LVGL
  检查也通过，独立代码复核未发现剩余问题。
- Device tests：**此版本 NOT RUN**。ESP32-C3 已连接于 `/dev/cu.usbmodem1101`，
  设备仍运行上次授权的车牌版，没有未经本版确认就写入新镜像。
- Unverified：实机 60 秒时序、背光关闭和唤醒、唤醒手势隔离、设备上的草稿保留、
  实际省电幅度、断电里程恢复、电量精度、运行时堆内存与任务栈，以及待确认的 Polo 参数。

合并固件：`build/firmware/b00454b2adc1c32e2ca79d9b24c50452239c3a49f84163276d66229e8cb64df8/FoloToy-AI-Passport-full.bin`，
**963,904 字节**，从 **0x0** 刷写；归档保留匹配 ELF、MAP 和已校验的清单。

```text
Full image SHA-256: b00454b2adc1c32e2ca79d9b24c50452239c3a49f84163276d66229e8cb64df8
Matching ELF SHA-256: 018a53bfd0b1ab638da5f82867267be0c951e49029f5d8ca9f2cc97cca2468d2
```

完整验证日志为 `build/carcard-idle-validation.log`，修改保留在 `feature/carcard`，未提交。
本版需再次获得明确烧录授权，合并刷写可能重置 NVS；分区表保持不变，归档有匹配组件镜像，
如果要求保留已有记录，可进行兼容的分段刷写。

## 上次交付：增加车牌，2026-10-06

两辆车的名片各自显示车主提供的对应车牌，使用蓝色牌框。
像素图下移，预留独立的车牌行；里程和操作方式不变。
三种字号的字符清单均已扩充至 179 个字形。

- Build：**PASS**，ESP-IDF v5.5.3，完整统一验证、合并镜像和归档校验通过。
- Host tests：**PASS**，仓库、模型、按键及存储检查、实际 LVGL 当前字形与文字边界、
  十种渲染状态和 300 次页面/车辆切换通过，已目视检查两车车牌对应关系。
- Device tests：**已观察项 PASS**。2026-10-06 车主明确授权刷入上述准确车牌版归档。
  在 `/dev/cu.usbmodem1101` 识别到唯一 ESP32-C3，内置 8 MB Flash；esptool v4.12.0
  从 `0x0` 写入 963,408 字节合并镜像，设备端数据哈希校验通过。
  写入扇区范围为 `0x00000000` 至 `0x000EBFFF`，未进行全片擦除。
  有界观察启动日志 25 秒，仅出现一次应用启动及 Ready，未出现 panic、看门狗或崩溃标记。
  设备上报的 ELF 哈希前缀与归档匹配，字体、存储、按键和电量模块均初始化成功，
  启动可用堆内存为 237,008 字节。车主确认两车车牌、中文、像素车图、换车、切页、
  进入与取消里程编辑均正常，观察结束后已关闭串口。
- Unverified：保存里程后的断电恢复、电量精度、长时间使用时的堆内存、任务栈余量、
  长时间稳定性，以及先前待确认的 Polo 参数。
- 主机 LVGL 24 KB 内存池：峰值 17,800 字节，稳定后可用 6,816 字节，
  最大空闲块 6,360 字节；这不是设备堆内存测量。

合并固件：`build/firmware/179810e5a8280ae00795125f7c7ec0074e22949620ada3502032749856fc7b7f/FoloToy-AI-Passport-full.bin`，
**963,408 字节**，从 **0x0** 刷写。相同归档保存匹配的 ELF、MAP 和已校验的清单。

```text
Full image SHA-256: 179810e5a8280ae00795125f7c7ec0074e22949620ada3502032749856fc7b7f
Matching ELF SHA-256: 211bb70ae965285e9d801178e2ab247b9276dc025a564f974eb2620a22dffe40
```

验证日志为 `build/carcard-plates-validation.log`，主机预览为
`build/carcard-plates-preview.png`。修改保留在本地 `feature/carcard`，未提交。
本次准确合并镜像已在明确授权后烧录，NVS 位于合并写入范围内。
本地原始日志保存在 `build/carcard-plates-flash.log` 和
`build/carcard-plates-startup.log`，不用于公开发布。

## 上次交付：无车牌的两车车库，2026-10-06

- Build：**PASS**，ESP-IDF v5.5.3，完整统一验证、合并布局及归档校验通过。
- Host tests：**PASS**，包括仓库与基线检查、两车切换及编辑、BSP 事件转换、
  独立 NVS 记录及旧 R36 兼容、错误与模拟重启、三种字号全部 177 字形、
  每个标签当前字体与边界、十种渲染状态及四种普通页面间 300 次切换。
- Device tests：**NOT RUN**，当前 macOS USB 清单未发现 Espressif 候选设备；
  未打开串口，未烧录。
- Unverified：屏幕实显、颜色和中文、实体按键、电量、真实 NVS 保存与断电行为、
  运行时堆内存和任务栈，以及 Polo 年份、代号和准确发动机功率。插画按暂定 9N3 绘制。
- 主机 LVGL 24 KB 内存池：峰值分配 17,808 字节，稳定后可用 7,272 字节，
  最大空闲块 6,840 字节；这是主机内存池结果，并非设备堆内存测量。
- 固件 MAP：静态 D/IRAM 104,190 字节（剩余 217,106 字节），Flash 段 822,210 字节；
  应用镜像 896,944 字节，继续使用原有 8 MB 布局。

合并固件：`build/firmware/4f1dbd84e38fb7415b3f089fb17956afd24db36cbe16666d5c23cd2ecb3050d6/FoloToy-AI-Passport-full.bin`，
**962,480 字节**，从 **0x0** 刷写。归档同时保存匹配的 ELF、MAP、应用、
bootloader、分区镜像、刷写参数及已校验的清单。

```text
Full image SHA-256: 4f1dbd84e38fb7415b3f089fb17956afd24db36cbe16666d5c23cd2ecb3050d6
Matching ELF SHA-256: e478071efbe90401f7245b0432c46d20e4cbdbc0f9d0063a5f1e4c94fa4be66a
```

验证日志为 `build/carcard-garage-validation.log`，当前主机预览为
`build/carcard-garage-preview.png`。修改保留在 `feature/carcard`，未提交；
未获授权进行提交、发布或烧录。合并刷写可能重置 NVS；兼容的分段刷写可在
[刷写政策](../development/engineering/firmware-layout.zh_CN.md)范围内保留已有 R36 记录。

## 上次交付：R36 单车版，2026-10-06

- Build：**PASS**，ESP-IDF v5.5.3，统一固件验证入口及合并镜像校验通过。
- Host tests：**PASS**，包括仓库检查、基线故障测试、CarCard 状态模型、BSP 事件转换、
  NVS 错误及重启契约、实际 LVGL 字形和文字边界检查、七种渲染状态、300 次翻页。
  NVS 故障桩反映 setter 立即写入的行为；提交失败只验证错误处理，不假设回滚。
- Device tests：**NOT RUN**，未发现 Espressif USB 设备，未打开串口，未烧录固件。
- Unverified：屏幕实显、颜色与中文、实体按键时序、真实断电和重启时的 NVS 行为、
  电量读数、运行时堆内存及任务栈。
- 主机 LVGL 内存池为 24 KB，峰值分配 17,816 字节，稳定后可用 3,592 字节，
  最大空闲块 2,984 字节。主机指针大小与 ESP32 不同，这不是设备堆内存测量结果。

历史合并镜像保存在下方归档中，**915,168 字节**，刷写地址 **0x0**。
匹配的 ELF、MAP 和清单保存在
`build/firmware/4be8b5cb0116913ff7fca28af42066da5353b3cc879b32b3609ef8e561fe3728/`。

```text
Full image SHA-256: 4be8b5cb0116913ff7fca28af42066da5353b3cc879b32b3609ef8e561fe3728
Matching ELF SHA-256: c6f45978b72815e0ac3d6badb13d5f43d6837902db96f94acc065d64dbb076d2
```

修改保留在 `feature/carcard`，未提交。车主未要求 Git 提交或发布。
界面预览位于 `build/carcard-ui-preview.png`。

交付合并固件从 **0x0** 刷写，可能重置 NVS，包括原有设置和修改后的里程。
遵循[刷写政策](../development/engineering/firmware-layout.zh_CN.md)，
必须获得车主针对本次准确镜像的明确同意才烧录。
