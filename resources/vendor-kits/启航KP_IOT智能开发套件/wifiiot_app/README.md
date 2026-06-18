# wifiiot_app — OpenHarmony 应用层代码

本目录为启航 KP-IOT 智能开发套件的 OpenHarmony 应用层代码框架，包含 12 个传感器/功能模块的完整驱动与应用实现。

## 目录结构

```
wifiiot_app/
├── src/                     # 主程序入口
│   ├── app_main.c           # 应用主入口，任务创建与调度
│   ├── app_main.h           # 应用主入口头文件
│   ├── ohos_main.c          # OpenHarmony 系统主函数
│   ├── ohos_main.h          # 系统主函数头文件
│   ├── app_demo_uart.c      # UART 演示代码（AT 指令）
│   ├── app_demo_uart.h      # UART 演示头文件
│   ├── app_demo_upg_verify.c # 升级验证
│   ├── app_demo_upg_verify.h # 升级验证头文件
│   └── SConscript           # SCons 构建脚本
│
├── include/                 # 公共头文件
│   ├── app_io_init.h        # IO 初始化接口声明
│   └── app_main.h           # 应用主接口声明
│
├── init/                    # IO 初始化
│   ├── app_io_init.c        # GPIO/UART/I2C/SPI 等外设 IO 复用配置
│   └── SConscript           # SCons 构建脚本
│
├── application/             # 传感器模块应用层
│   ├── oled_module/         # OLED 显示模块（SPI，SH1106）
│   ├── motor_module/        # 电机驱动 + 温湿度传感器（SHT30，I2C）
│   ├── smokeAlarm_module/   # 烟雾报警模块（ADC）
│   ├── combustibleGas_module/  # 可燃气体检测模块（ADC）
│   ├── humanbodyIdentify_module/ # 人体红外识别模块（ADC）
│   ├── heartRate_module/    # 心率检测模块（MAX30102，I2C）
│   ├── gps_module/          # GPS 定位模块（UART）
│   ├── intelligentVoice_module/ # 智能语音模块（UART）
│   ├── nfc_module/          # NFC 读写模块（NT3H，I2C）
│   ├── smartDoor_module/    # 智能门锁模块（指纹，UART）
│   └── smartFitness_module/ # 智能健身模块（三轴加速度，I2C）
│
├── app.json                 # 应用配置（模块使能/禁用）
├── Makefile                 # 顶层 Makefile
├── SConscript               # 顶层 SCons 构建脚本
└── module_config.mk         # 模块编译选项配置
```

## 构建配置说明

本目录包含 OpenHarmony 工程中常见的构建配置文件。实际编译方式请以课程环境、开发板资料和所集成的 OpenHarmony 工程为准。

| 文件 | 作用 |
|------|------|
| `SConscript`（顶层） | 汇总 `src/`、`init/`、`application/` 各子目录的编译目标 |
| `SConscript`（子目录） | 声明当前目录的源文件与编译规则 |
| `Makefile` | 模块构建入口文件，具体调用方式以课程工程为准 |
| `app.json` | 声明应用包含的模块列表 |
| `module_config.mk` | 模块级编译选项（宏定义、链接库等） |

### 模块编译配置

`module_config.mk` 中配置各模块的编译开关：

```makefile
# 示例：使能/禁用 OLED 模块
OLED_MODULE = y
# MOTOR_MODULE = n
```

## 各模块文件说明

每个模块目录通常包含以下文件：

| 文件类型 | 说明 |
|----------|------|
| `*_module.c / *_module.h` | 模块主逻辑，任务创建与数据处理 |
| `*_i2c.c / *_i2c.h` | I2C 总线驱动封装 |
| `*_adc.c / *_adc.h` | ADC 采集驱动封装 |
| `*_uart.c / *_uart.h` | UART 通信驱动封装 |
| `dbg.c / dbg.h` | 调试日志宏定义 |
| `SConscript` | 模块构建脚本 |

## 集成流程

1. 在 `init/app_io_init.c` 中配置所需外设的 IO 复用
2. 在 `app.json` 或 `module_config.mk` 中使能目标模块
3. 编写/修改 `application/<module>/` 下的模块代码
4. 在 `src/app_main.c` 中创建模块任务
5. 按课程环境或 OpenHarmony 工程要求完成编译
6. 按开发板资料完成烧录并观察运行结果

## 适用平台

- **硬件**：启航 KP-IOT 智能开发套件（Hi3861 芯片 + E53 扩展板）
- **系统**：OpenHarmony 轻量系统
- **编译工具**：DevEco Device Tool / SCons

## 相关链接

- [启航 KP-IOT 套件总览](../)
- [OLED 独立示例](../022_OH_KP_OLED_SAMPLE/)
- [学习路径指南](../../docs/learning-path.md)
- [返回仓库首页](../../README.md)
