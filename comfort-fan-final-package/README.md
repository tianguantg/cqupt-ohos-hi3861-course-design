# 启航舒适风扇 v6 项目整理

这个目录放的是我整理出来的最终项目相关文件：设备端代码、编译烧录说明、MQTT 测试方法、简单前端入口和一些对照材料。项目基于 OpenHarmony Hi3861，实现温湿度采集、OLED 显示、虚拟风速控制、Wi-Fi + MQTT 双向通信和前端控制。

## 我实际按这个流程走

```text
合入 openharmony_patch/
→ 配置 network_config.h
→ 自己编译 allinone 固件
→ HiBurn 烧录
→ mosquitto_sub/pub 验证 MQTT
→ 基于 templates/simple-dashboard/ 编写或改造前端
```

仓库中保留了成品固件和完整 dashboard 参考实现，用于验收、故障对照和代码参考。正常复现不依赖这些成品资源。

## 主要功能

- SHT30 温湿度采集
- SH1106 OLED 本地显示
- OFF / QUIET / AUTO / ECO 模式
- 温度 + 湿度共同影响 AUTO 风速
- 虚拟风速 5% 平滑变化
- MQTT 状态 1 秒周期上报
- MQTT 远程命令控制
- MQTT 命令行控制
- 简单 Web 前端或自定义前端控制
- 默认电机低功率常转；也可切换为“敏感脉冲反馈”模式，现场噪音较大时建议关闭常转

## 目录

```text
firmware/           验收和故障对照用固件
openharmony_patch/  OpenHarmony 应用源码和配置模板
dashboard/          完整 Web 控制台参考实现
scripts/            编译、复制、Broker、测试脚本
docs/               编译烧录、测试、硬件和验收文档
evidence/           构建和串口/MQTT 日志样例
```

## 建议阅读顺序

1. [最终项目联调指南](./docs/最终项目联调指南.md)
2. [编译烧录指南](./docs/编译烧录指南.md)
3. [MQTT 测试指南](./docs/MQTT测试指南.md)
4. [简单前端模板](../templates/simple-dashboard/README.md)
5. [Web 控制台使用指南](./docs/Web控制台使用指南.md)：完整参考实现，可选
6. [实机验收记录](./docs/实机验收记录.md)

## 注意

`network_config.example.h` 只是模板，不包含真实 Wi-Fi 密码。真实 `network_config.h` 只放在自己的编译环境里，不要提交到公开仓库。
