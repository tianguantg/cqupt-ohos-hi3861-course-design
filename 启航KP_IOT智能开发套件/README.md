# 启航 KP-IOT 智能开发套件

本目录包含启航 KP-IOT 智能开发套件的完整软硬件开发资料，基于华为海思 Hi3861 芯片，运行 OpenHarmony 轻量系统。

## 目录结构

```
启航KP_IOT智能开发套件/
├── wifiiot_app/                      # 应用层代码框架（12 个传感器模块）
│   ├── application/                  # 各传感器模块实现
│   │   ├── combustibleGas_module/    # 可燃气体检测
│   │   ├── gps_module/               # GPS 定位
│   │   ├── heartRate_module/         # 心率检测（MAX30102）
│   │   ├── humanbodyIdentify_module/ # 人体红外识别
│   │   ├── intelligentVoice_module/  # 智能语音
│   │   ├── motor_module/             # 电机驱动 + 温湿度（SHT30）
│   │   ├── nfc_module/               # NFC 读写
│   │   ├── oled_module/              # OLED 显示（SH1106）
│   │   ├── smartDoor_module/         # 智能门锁（指纹）
│   │   ├── smartFitness_module/      # 智能健身（三轴加速度）
│   │   └── smokeAlarm_module/        # 烟雾报警
│   ├── include/                      # 公共头文件
│   ├── init/                         # IO 初始化
│   ├── src/                          # 主程序入口
│   ├── app.json                      # 应用配置文件
│   ├── Makefile                      # 构建 Makefile
│   ├── SConscript                    # SCons 构建脚本
│   └── module_config.mk              # 模块编译配置
│
├── OHOS/                             # OpenHarmony 硬件抽象层头文件
│   ├── wifiiot_gpio.h               # GPIO 接口
│   ├── wifiiot_gpio_ex.h            # GPIO 扩展接口
│   ├── wifiiot_i2c.h                # I2C 接口
│   ├── wifiiot_i2c_ex.h             # I2C 扩展接口
│   └── wifiiot_errno.h              # 错误码定义
│
├── 022_OH_KP_OLED_SAMPLE/            # OLED 显示模块独立示例
│   ├── include/                      # 头文件
│   ├── *.c                           # 源码文件
│   ├── BUILD.gn                      # GN 构建文件
│   └── README.md                     # 示例说明
│
├── 022_OH_KP_OLED_SAMPLE.zip         # OLED 示例代码包
├── 023_OH_KP_SHT30_Agriculture.zip   # SHT30 温湿度示例代码包
├── code-master.zip                   # HelloWorld 基础项目模板
│
├── OLED显示模块实验手册.docx          # OLED 实验指导手册
├── OpenHarmony物联网开发指南.docx     # 物联网开发综合指南
├── OpenHarmony系统传感器-温湿度检测 - 指导手册.docx  # 温湿度检测实验手册
│
├── 鸿湖万联 启航 KP-IOT套件介绍.pdf    # 套件产品介绍
├── 软通启航KP_IOT物联网开发套件快速入门手册v2.0.pdf  # 快速入门手册
├── 附件--Schematic_E53标准板-2-电机+温湿度-V0.4_2022-06-21.pdf  # E53 标准板原理图
├── 附件--SHT30-DataSheet.PDF          # SHT30 传感器数据手册
│
├── 配套代码说明.txt                   # 代码配套说明
├── 启航KP_IOT开发板环境搭建（参考网址）.txt  # 环境搭建参考链接
├── 学习视频链接.txt                   # 视频教程链接
└── 开发软件下载地址.txt               # 开发工具下载地址
```

## 文件类型说明

| 类型 | 用途 | 推荐工具 |
|------|------|----------|
| `.c` / `.h` | C 语言源代码与头文件 | 代码编辑器 / IDE |
| `.gn` / `.json` / `.mk` | 构建配置文件 | 代码编辑器 |
| `.docx` | 实验手册、开发指南 | Microsoft Word / WPS |
| `.pdf` | 数据手册、原理图、产品介绍 | PDF 阅读器 |
| `.zip` | 独立示例代码包 | 解压后使用 |
| `.txt` | 说明文档、参考链接 | 文本编辑器 |

## 硬件规格

- **主控芯片**：华为海思 Hi3861V100
- **操作系统**：OpenHarmony 轻量系统
- **通信接口**：GPIO / I2C / SPI / UART / ADC / PWM
- **扩展板**：E53 标准板（含多种传感器）

## 快速开始

1. 阅读 `软通启航KP_IOT物联网开发套件快速入门手册v2.0.pdf` 了解硬件
2. 参考 `启航KP_IOT开发板环境搭建（参考网址）.txt` 搭建编译环境
3. 使用 `code-master.zip` 验证环境（编译 → 烧录 → 运行）
4. 阅读 `OpenHarmony物联网开发指南.docx` 了解开发流程
5. 进入 `wifiiot_app/` 目录，选择感兴趣的模块开始学习

## 传感器模块概览

| 序号 | 模块 | 目录 | 通信方式 | 核心器件 |
|------|------|------|----------|----------|
| 1 | OLED 显示 | `application/oled_module/` | SPI | SH1106 |
| 2 | 温湿度 | `application/motor_module/` | I2C | SHT30 |
| 3 | 电机驱动 | `application/motor_module/` | GPIO/PWM | — |
| 4 | 烟雾报警 | `application/smokeAlarm_module/` | ADC | — |
| 5 | 可燃气体 | `application/combustibleGas_module/` | ADC | — |
| 6 | 人体红外 | `application/humanbodyIdentify_module/` | ADC | — |
| 7 | 心率检测 | `application/heartRate_module/` | I2C | MAX30102 |
| 8 | GPS 定位 | `application/gps_module/` | UART | — |
| 9 | 智能语音 | `application/intelligentVoice_module/` | UART | — |
| 10 | NFC 读写 | `application/nfc_module/` | I2C | NT3H |
| 11 | 智能门锁 | `application/smartDoor_module/` | UART | 指纹模块 |
| 12 | 智能健身 | `application/smartFitness_module/` | I2C | 三轴加速度 |

## 相关链接

- [返回仓库首页](../README.md)
- [应用代码说明](./wifiiot_app/)
- [学习路径指南](../docs/learning-path.md)
- [常见问题](../FAQ.md)
