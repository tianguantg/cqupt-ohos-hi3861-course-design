# 启航舒适风扇 v6 最终包

本包用于课程展示、复现和归档。项目基于 OpenHarmony Hi3861，实现温湿度采集、OLED 显示、虚拟风速控制、Wi-Fi + MQTT 双向通信和 Web 远程控制台。

## 当前最终固件

```text
firmware/qihang_comfort_fan_mqtt_v6_fast_report_allinone.bin
SHA256: A7D6A890E916988BF489D9C2936A4A9F849418698423EB1F4622A1A7E74B08AC
```

## 主要功能

- SHT30 温湿度采集
- SH1106 OLED 本地显示
- OFF / QUIET / AUTO / ECO 模式
- 温度 + 湿度共同影响 AUTO 风速
- 虚拟风速 5% 平滑变化
- MQTT 状态 1 秒周期上报
- MQTT 远程命令控制
- Web 控制台和手机局域网访问
- 默认电机低功率常转；也可切换为“敏感脉冲反馈”模式，现场噪音较大时建议关闭常转

## 目录

```text
firmware/           最终固件和校验
openharmony_patch/  OpenHarmony 应用源码和配置模板
dashboard/          Web 控制台
scripts/            编译、复制、Broker、测试脚本
docs/               编译烧录、测试、硬件和验收文档
evidence/           构建和串口/MQTT 日志样例
```

## 推荐阅读顺序

1. [最终项目联调指南](./docs/最终项目联调指南.md)
2. [编译烧录指南](./docs/编译烧录指南.md)
3. [MQTT 测试指南](./docs/MQTT测试指南.md)
4. [Web 控制台使用指南](./docs/Web控制台使用指南.md)
5. [实机验收记录](./docs/实机验收记录.md)

## 注意

`network_config.example.h` 是模板，不包含真实 Wi-Fi 密码。真实 `network_config.h` 不应提交到公开仓库。
