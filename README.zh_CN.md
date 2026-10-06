[English](README.md) · **简体中文**

# CarCard

为 **FoloToy AI Passport** 开发的离线像素车库，基于完整的
[FoloToy/ai-passport](https://gitee.com/FoloToy/ai-passport) 项目。
应用维护在 `feature/carcard` 分支，复用原项目 BSP。

当前版本暂时仅内置作者已有的 R36 和 Polo，尚不支持在应用中新增其他车型。
下一版计划加入常见车辆信息；这些资料尚未包含在本次版本中。

<p align="center">
  <img src="assets/images/r36-silver-variant.png" alt="银色大众 R36 旅行版像素插画" width="324">
  <img src="assets/images/polo-silver-9n3.png" alt="银色大众 Polo 两厢像素插画" width="324">
</p>

## 功能

- 银色 R36 旅行版与银色 2008 年 Polo 两厢版的独立车辆名片。
- 像素车辆插画、中文界面、原厂参数及电量显示。
- 两辆车独立编辑里程，使用 NVS 保存并在重启后恢复。
- 60 秒无操作关闭背光，任意按键唤醒。
- 公开源码使用示例车牌，可通过本地私有配置覆盖。

| 操作 | 车辆名片 / 参数页 | 里程编辑 |
| --- | --- | --- |
| 上 / 下 | 切换页面 | 当前数字增加 / 减少 |
| 短按确认 | 换车，保持页面类型 | 下一位；最后一位保存 |
| 长按确认 | 编辑当前车辆里程 | 取消修改 |

熄屏后的首次手势仅唤醒屏幕。车辆信息为手动记录，应用不读取车辆实时数据。

## 硬件与开发

ESP32-C3、8 MB Flash、无 PSRAM、240 × 320 屏幕及三枚功能按键。
使用 **ESP-IDF 5.5.3**。AI 开发前阅读 [AGENTS.md](AGENTS.md)；
项目提供 [skills/](skills/README.zh_CN.md) 下的五个 Passport Skill。

1. 按[环境指南](docs/development/engineering/environment-setup.zh_CN.md)准备工具链。
2. 使用 ESP-IDF 的 `export.sh` 激活环境。
3. 运行完整验证：

```bash
./tools/validate.sh
```

验证包括主机测试、固件构建、合并镜像校验及实际 LVGL 页面渲染，检查字体和文字边界。
单独检查可使用：

```bash
./tools/validate.sh --static
./tools/validate.sh --firmware
```

验证通过的 `build/FoloToy-AI-Passport-full.bin` 可从 **0x0** 初始化设备。
匹配的 ELF、MAP 和分段镜像归档在 `build/firmware/<sha256>/`。
构建产物和原始日志不纳入 Git。合并刷写可能重置 NVS；如需保留里程，
使用分区兼容且匹配的分段镜像。详见[刷写政策](docs/development/engineering/firmware-layout.zh_CN.md#烧录与已存数据)。

## 私有车牌配置

公开版本显示 `R36 DEMO` 和 `POLO DEMO`。本地个性化构建可创建
`main/carcard_local_config.h`：

```c
#pragma once
#define CARCARD_R36_PLATE "YOUR_R36_PLATE"
#define CARCARD_POLO_PLATE "YOUR_POLO_PLATE"
```

此文件已被 Git 忽略。编译时定义 `CARCARD_PUBLIC_BUILD=1` 可禁用本地覆盖，
用于公开预览。自定义中文必须包含在生成的字体子集中，参见[字体指南](docs/development/engineering/lvgl-chinese-fonts.zh_CN.md)。

## 验证与说明

固件构建、主机测试及 LVGL 字体/布局检查已通过。车主已在设备上确认参数显示、
中文、里程以及切页/换车按键正常。熄屏/唤醒、实际省电效果及断电里程恢复
仍需实机验收。Polo 发动机系列标签由车主提供。数据来源、准确固件标识及
未验证项详见[应用指南](docs/apps/carcard.zh_CN.md)。

代码使用 [MIT 许可证](LICENSE)。字体子集保留
[SIL Open Font License](assets/fonts/OFL.txt)，图片来源及生成方法见[素材指南](assets/README.zh_CN.md)。
