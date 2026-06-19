# Final Firmware

本目录放学生可直接烧录的最终固件。

```text
qihang_comfort_fan_mqtt_v6_fast_report_allinone.bin
```

SHA256：

```text
A7D6A890E916988BF489D9C2936A4A9F849418698423EB1F4622A1A7E74B08AC
```

功能对应：

```text
启航舒适风扇 v6
温湿度采集
OLED 显示
MQTT 状态上报
Web 控制台远程控制
默认 Motor ON 电机低功率常转
MOTOR_OFF 关闭常转但保留敏感脉冲反馈
如果电机转动噪音太大，可以通过 Web 控制台或 MQTT 发送 MOTOR_OFF 关闭常转。
```

烧录工具：

```text
resources/tools/HiBurn.exe
```

烧录流程见：

- [通用配置步骤](../../docs/course-design/01_通用配置步骤.md)
- [编译烧录指南](../../comfort-fan-final-package/docs/编译烧录指南.md)
