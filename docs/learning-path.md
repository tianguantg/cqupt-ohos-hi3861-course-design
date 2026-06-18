# 学习路径指南

本指南为 OpenHarmony 智能设备开发提供建议的学习路线，适用于初学者和有一定嵌入式基础的开发者。

---

## 第一阶段：环境搭建与 Hello World（预计 2-3 天）

### 目标
完成开发环境安装，跑通第一个程序的编译、烧录、运行全流程。

### 学习内容
1. 安装 DevEco Device Tool
2. 配置 Hi3861 编译工具链
3. 安装 CH340 串口驱动
4. 使用 `code-master.zip`（HelloWorld）验证环境

### 对应资料
- `resources/vendor-kits/resources/vendor-kits/bossay开发板/第1章 C语言HarmonyOS设备开发环境.pptx`
- `resources/vendor-kits/resources/vendor-kits/bossay开发板/HarmonyOS设备开发环境搭建.pptx`
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/code-master.zip`

---

## 第二阶段：C 语言设备开发基础（预计 3-5 天）

### 目标
掌握 OpenHarmony 设备端 C 语言编程的基本方法和规范。

### 学习内容
1. OpenHarmony 设备程序结构（入口、任务、事件循环）
2. GPIO 输入输出控制
3. 延时、日志打印、调试方法
4. SCons 构建系统基础

### 对应资料
- `resources/vendor-kits/resources/vendor-kits/bossay开发板/第2章 C语言HarmonyOS设备程序开发.pptx`
- `resources/vendor-kits/resources/vendor-kits/bossay开发板/第3章 C语言HarmonyOS设备程序开发方法、步骤.pptx`
- `resources/vendor-kits/resources/vendor-kits/bossay开发板/实验用源码（适配resources/vendor-kits/bossay开发板）.docx`

---

## 第三阶段：外设通信协议（预计 3-5 天）

### 目标
掌握常用外设通信协议，为传感器开发打基础。

### 学习内容
1. **GPIO**：通用输入输出，控制 LED、按键、继电器
2. **I2C**：掌握 I2C 总线时序，阅读设备数据手册（参考 SHT30 DataSheet）
3. **SPI**：掌握 SPI 总线时序，驱动 OLED 显示屏（参考 SH1106）
4. **UART**：掌握串口通信，处理 GPS、语音模块数据
5. **ADC**：模拟数字转换，读取烟雾/可燃气体传感器

### 对应资料
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/OHOS/` 目录下的 HAL 接口头文件（`wifiiot_gpio.h`、`wifiiot_i2c.h` 等）
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/附件--SHT30-DataSheet.PDF`
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/附件--Schematic_E53标准板...pdf`

---

## 第四阶段：传感器模块实战（预计 7-10 天）

### 目标
逐个学习各传感器模块的驱动开发和数据处理。

### 推荐顺序

| 顺序 | 模块 | 目录 | 涉及技术 |
|------|------|------|----------|
| 1 | OLED 显示 | `application/oled_module/` | SPI、字库、GUI 绘图 |
| 2 | 温湿度传感器 | `application/motor_module/` | I2C、SHT30 数据解析 |
| 3 | 电机驱动 | `application/motor_module/` | GPIO、PWM |
| 4 | 烟雾报警 | `application/smokeAlarm_module/` | ADC |
| 5 | 可燃气体检测 | `application/combustibleGas_module/` | ADC |
| 6 | 人体红外识别 | `application/humanbodyIdentify_module/` | ADC |
| 7 | 心率检测 | `application/heartRate_module/` | I2C、MAX30102 算法 |
| 8 | GPS 定位 | `application/gps_module/` | UART、NMEA 协议解析 |
| 9 | 智能语音 | `application/intelligentVoice_module/` | UART |
| 10 | NFC 读写 | `application/nfc_module/` | I2C、NDEF 协议 |
| 11 | 智能门锁 | `application/smartDoor_module/` | 指纹模块、综合应用 |
| 12 | 智能健身 | `application/smartFitness_module/` | I2C、三轴加速度 |

### 学习方法
1. 阅读模块目录下的 `README.txt` 或源码注释
2. 理解硬件接线（参考 E53 标准板原理图）
3. 阅读设备数据手册（DataSheet）
4. 阅读源码中的初始化、数据采集、任务处理流程
5. 编译烧录，通过串口查看日志验证功能

### 对应资料
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/OLED显示模块实验手册.docx`
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/OpenHarmony物联网开发指南.docx`
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/OpenHarmony系统传感器-温湿度检测 - 指导手册.docx`
- `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/022_OH_KP_OLED_SAMPLE/README.md`

---

## 第五阶段：综合项目实战（预计 5-7 天）

### 目标
整合多个模块，完成一个完整的物联网应用。

### 项目建议
1. **智能家居环境监测**：温湿度 + 烟雾 + 可燃气体 → OLED 显示 + 蜂鸣器报警
2. **智能门禁系统**：指纹模块 + NFC + 电机（门锁控制）→ OLED 显示状态
3. **健康监测终端**：心率 + 三轴加速度 → OLED 显示数据 → 语音播报
4. **户外定位追踪**：GPS + 温湿度 → OLED 显示 → NFC 数据导出

---

## 关键资源索引

| 资源 | 位置 |
|------|------|
| 环境搭建课件 | `resources/vendor-kits/resources/vendor-kits/bossay开发板/` |
| C 语言开发课件 | `resources/vendor-kits/resources/vendor-kits/bossay开发板/` |
| HAL 接口头文件 | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/OHOS/` |
| 实验手册（DOCX） | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/*.docx` |
| 硬件数据手册（PDF） | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/附件--*.pdf` |
| 传感器模块源码 | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/wifiiot_app/application/` |
| OLED 示例代码 | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/022_OH_KP_OLED_SAMPLE/` |
| HelloWorld 模板 | `resources/vendor-kits/resources/vendor-kits/启航KP_IOT智能开发套件/code-master.zip` |

---

## 常见问题

遇到问题请查看 [FAQ.md](../FAQ.md) 或通过 GitHub Issues 提问。
