# MQTT 测试指南

## 主题

```text
状态：comfortfan/qihang01/state
命令：comfortfan/qihang01/cmd
事件：comfortfan/qihang01/event
```

## 支持命令

```text
OFF
QUIET
AUTO
ECO_ON
ECO_OFF
MORE
LESS
MOTOR_ON
MOTOR_OFF
STATUS
```

说明：

```text
默认 Motor Hold 为 ON。
MOTOR_ON：虚拟风速 > 0 时电机最低功率常转。
MOTOR_OFF：关闭常转，但保留等级变化或 5% 风速变化触发的敏感脉冲反馈。
如果电机常转噪音太大，发送 MOTOR_OFF 可以降噪；需要恢复常转时发送 MOTOR_ON。
```

## Windows 订阅

```powershell
Set-Location "C:\Program Files\mosquitto"
.\mosquitto_sub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/#" -v
```

## Windows 发布

```powershell
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "AUTO"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "MOTOR_ON"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "STATUS"
```
