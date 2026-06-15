# 鸿蒙OS智能设备开发项目选题方案

> 本文档基于《硬件综合设计开发课程设计指导书（2026）》和 `docs/learning-path.md` 整理，所有模块均来自 `启航KP_IOT智能开发套件/wifiiot_app/application/` 实际源码目录。

---

## 选题原则

- 每组 **2~3 人**，需完成从选题、方案设计、开发、测试到现场答辩的全过程。
- 项目需基于 **Hi3861 开发板**，使用 **C 语言** 开发。
- 要求接入 **MQTT + 物联网云平台**（腾讯云 IoT Explorer / 阿里云 IoT 平台 / OneNET）。
- 可自选课题，但需授课教师审核通过。

---

## 方案一：智能家居环境监测系统

**核心目标**：实时监测室内环境数据，异常时本地报警并上传云端。

| 项目 | 说明 |
|---|---|
| 涉及模块 | OLED 显示、温湿度 SHT30、烟雾检测、可燃气体检测 |
| 参考代码位置 | `application/oled_module/`、`application/motor_module/`（含 `sht3x_i2c.c`）、`application/smokeAlarm_module/`、`application/combustibleGas_module/` |
| 主要功能 | ① OLED 实时显示温湿度、烟雾浓度、可燃气体浓度；② 温度或气体浓度超过阈值时蜂鸣器报警；③ 通过 MQTT 将数据上报云平台，支持远程查看 |
| 云平台接入 | 使用 MQTT 协议连接腾讯云/阿里云/OneNET，上报传感器数据，接收远程控制指令 |
| 推荐人数 | 2~3 人 |

---

## 方案二：智能门禁系统

**核心目标**：实现多种身份验证方式，控制门锁开关并记录状态。

| 项目 | 说明 |
|---|---|
| 涉及模块 | 智能门锁、NFC、OLED 显示、人体红外识别 |
| 参考代码位置 | `application/smartDoor_module/`、`application/nfc_module/`、`application/oled_module/`、`application/humanbodyIdentify_module/` |
| 主要功能 | ① 支持 NFC 刷卡或指纹验证开门；② OLED 显示门锁状态（关/开/验证中）；③ 人体红外检测到有人靠近时自动唤醒验证；④ 开门事件通过 MQTT 上报云端 |
| 云平台接入 | 使用 MQTT 上报开门记录、异常闯入报警，支持远程查看门禁日志 |
| 推荐人数 | 2~3 人 |

---

## 方案三：健康监测终端

**核心目标**：采集人体生理数据，本地显示并支持语音播报。

| 项目 | 说明 |
|---|---|
| 涉及模块 | 心率检测、智能健身（三轴加速度）、OLED 显示、智能语音 |
| 参考代码位置 | `application/heartRate_module/`、`application/smartFitness_module/`、`application/oled_module/`、`application/intelligentVoice_module/` |
| 主要功能 | ① 实时采集心率和运动加速度数据；② OLED 显示心率、步数等信息；③ 异常心率时语音模块播报提醒；④ 数据可通过 MQTT 上传云端存储 |
| 云平台接入 | 使用 MQTT 上报心率、运动数据，支持历史数据查询 |
| 推荐人数 | 2~3 人 |

---

## 方案四：户外定位追踪终端

**核心目标**：获取位置信息，结合环境数据展示，并支持近场数据导出。

| 项目 | 说明 |
|---|---|
| 涉及模块 | GPS 定位、温湿度 SHT30、OLED 显示、NFC |
| 参考代码位置 | `application/gps_module/`、`application/motor_module/`（含 `sht3x_i2c.c`）、`application/oled_module/`、`application/nfc_module/` |
| 主要功能 | ① GPS 解析 NMEA 协议获取经纬度；② OLED 显示当前位置、温湿度；③ 通过 MQTT 将位置和温湿度数据上报云平台；④ NFC 可用于导出当前位置信息 |
| 云平台接入 | 使用 MQTT 上报 GPS 坐标和环境数据，云端可在地图上展示轨迹 |
| 推荐人数 | 2~3 人 |

---

## 方案五：智能风扇与健身监测系统

**核心目标**：结合运动检测自动控制风扇，实现简单的智能健身场景。

| 项目 | 说明 |
|---|---|
| 涉及模块 | 智能健身（三轴加速度）、电机驱动（风扇）、OLED 显示、心率检测 |
| 参考代码位置 | `application/smartFitness_module/`、`application/motor_module/`、`application/oled_module/`、`application/heartRate_module/` |
| 主要功能 | ① 通过加速度检测运动状态；② 根据运动强度自动调节风扇转速；③ OLED 显示心率、运动量和风扇状态；④ 数据通过 MQTT 上报 |
| 云平台接入 | 使用 MQTT 上报运动数据和风扇状态，支持远程控制风扇开关 |
| 推荐人数 | 2~3 人 |

---

## 模块速查表

| 模块 | 实际目录 | 关键技术 |
|---|---|---|
| OLED 显示 | `application/oled_module/` | SPI、字库、GUI 绘图 |
| 温湿度 SHT30 | `application/motor_module/` | I2C、SHT30 数据解析 |
| 电机驱动 | `application/motor_module/` | GPIO、PWM |
| 烟雾报警 | `application/smokeAlarm_module/` | ADC |
| 可燃气体检测 | `application/combustibleGas_module/` | ADC |
| 人体红外识别 | `application/humanbodyIdentify_module/` | ADC |
| 心率检测 | `application/heartRate_module/` | I2C、MAX30102 |
| GPS 定位 | `application/gps_module/` | UART、NMEA 协议解析 |
| 智能语音 | `application/intelligentVoice_module/` | UART |
| NFC 读写 | `application/nfc_module/` | I2C、NDEF 协议 |
| 智能门锁 | `application/smartDoor_module/` | 指纹模块、综合应用 |
| 智能健身 | `application/smartFitness_module/` | I2C、三轴加速度 |

---

## 选题后建议的开发顺序

1. **先跑通单个模块**：建议从 **OLED 显示** 开始，先学会看结果。
2. **再集成 2~3 个模块**：例如先实现“温湿度 + OLED”，再逐步加入烟雾/可燃气体。
3. **接入云平台**：最后通过 MQTT 把数据上传到腾讯云/阿里云/OneNET。
4. **撰写报告并准备答辩**：使用 `templates/` 下的报告模板。

---

## 参考文档

- [`docs/course-guide.md`](./course-guide.md) — 课程目标与要求
- [`docs/learning-path.md`](./learning-path.md) — 完整学习路线
- `启航KP_IOT智能开发套件/OpenHarmony物联网开发指南.docx`
- `启航KP_IOT智能开发套件/OLED显示模块实验手册.docx`
- `启航KP_IOT智能开发套件/OpenHarmony系统传感器-温湿度检测 - 指导手册.docx`
