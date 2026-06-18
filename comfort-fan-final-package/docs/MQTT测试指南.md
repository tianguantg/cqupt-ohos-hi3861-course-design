# MQTT 测试指南

本文负责验证设备端和 Windows MQTT Broker 是否连通。Web 控制台依赖这里的 MQTT 通路。

## 1. MQTT 代码在哪里

设备端：

```text
comfort-fan-final-package/openharmony_patch/fan_manager.c
comfort-fan-final-package/openharmony_patch/network_config.example.h
```

关键配置：

```c
#define CF_MQTT_URI        "tcp://192.168.137.1:1883"
#define CF_MQTT_USERNAME   ""
#define CF_MQTT_PASSWORD   ""
#define CF_DEVICE_ID       "qihang01"
```

关键函数：

| 函数 | 作用 |
|------|------|
| `BuildMqttTopics()` | 生成 `state`、`cmd`、`event` 三个主题 |
| `ConnectMqtt()` | 连接 Broker，订阅命令主题 |
| `MqttMessageArrived()` | 收到命令消息 |
| `HandleRemoteCommand()` | 解析并执行命令 |
| `PublishFanState()` | 发布设备状态 JSON |

## 2. 主题约定

默认设备 ID：

```text
qihang01
```

默认主题：

```text
状态：comfortfan/qihang01/state
命令：comfortfan/qihang01/cmd
事件：comfortfan/qihang01/event
```

如果 `network_config.h` 中修改了：

```c
#define CF_DEVICE_ID "team01"
```

则主题同步变为：

```text
comfortfan/team01/state
comfortfan/team01/cmd
comfortfan/team01/event
```

## 3. 启动 Mosquitto Broker

Windows PowerShell：

```powershell
Set-Location "C:\Program Files\mosquitto"
.\mosquitto.exe -v
```

看到类似输出表示 Broker 已启动：

```text
Opening ipv4 listen socket on port 1883.
mosquitto version ... running
```

如果设备在局域网访问这台电脑，需要放行端口：

```powershell
New-NetFirewallRule -DisplayName "Mosquitto MQTT TCP 1883" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1883 -Profile Any
```

## 4. 确认设备端 MQTT 地址

设备端 `network_config.h` 中的地址必须指向运行 Mosquitto 的电脑：

```c
#define CF_MQTT_URI "tcp://192.168.137.1:1883"
```

常见情况：

| 网络方式 | Broker 地址怎么填 |
|----------|-------------------|
| Windows 开移动热点，开发板连接该热点 | 通常填 `tcp://192.168.137.1:1883` |
| 电脑和开发板连接同一个路由器 | 填电脑在该 Wi-Fi 下的 IPv4 地址 |
| Broker 在其他电脑 | 填那台电脑的 IPv4 地址 |

Windows 查看 IP：

```powershell
ipconfig
```

修改 `network_config.h` 后必须重新编译并烧录。

## 5. 订阅设备状态

打开一个新的 Windows PowerShell：

```powershell
Set-Location "C:\Program Files\mosquitto"
.\mosquitto_sub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/#" -v
```

开发板上电或按 Reset 后，正常会看到：

```text
comfortfan/qihang01/event online
comfortfan/qihang01/state {"device":"qihang01","temperature":...,"humidity":...}
```

状态 JSON 常见字段：

| 字段 | 含义 |
|------|------|
| `device` | 设备 ID |
| `temperature` | 温度 |
| `humidity` | 湿度 |
| `mode` | `OFF` / `QUIET` / `AUTO` |
| `virtual_speed` | 虚拟风速，0-100 |
| `level` | `OFF` / `LOW` / `MEDIUM` / `HIGH` |
| `preference` | 舒适偏置，`MORE` / `LESS` 会改变它 |
| `eco` | `1` 表示 ECO 开启 |
| `motor_hold` | `1` 表示电机低功率常转，`0` 表示只保留脉冲反馈 |
| `wifi` | 设备端 Wi-Fi 状态 |
| `mqtt` | 设备端 MQTT 状态 |

## 6. 发布控制命令

再打开一个 Windows PowerShell：

```powershell
Set-Location "C:\Program Files\mosquitto"
```

发送命令：

```powershell
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "STATUS"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "AUTO"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "QUIET"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "OFF"
```

电机常转控制：

```powershell
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "MOTOR_ON"
.\mosquitto_pub.exe -h 127.0.0.1 -p 1883 -t "comfortfan/qihang01/cmd" -m "MOTOR_OFF"
```

如果电机转动噪音太大，发送 `MOTOR_OFF`。它会关闭低功率常转，但保留等级变化或 5% 虚拟风速变化时的敏感脉冲反馈。

## 7. 命令表

| 命令 | 设备端效果 |
|------|------------|
| `STATUS` | 立即请求一次状态上报 |
| `OFF` | 关闭风扇，虚拟风速归零 |
| `QUIET` | 安静模式，目标约 25%，ECO 时约 15% |
| `AUTO` | 自动模式，根据温湿度计算目标风速 |
| `ECO_ON` | 开启节能偏置，目标风速降低 |
| `ECO_OFF` | 关闭节能偏置 |
| `MORE` | 增加舒适偏置，提高目标风速 |
| `LESS` | 降低舒适偏置，降低目标风速 |
| `MOTOR_ON` | 开启低功率常转 |
| `MOTOR_OFF` | 关闭低功率常转，仅保留脉冲反馈 |

设备端也兼容部分变体，例如 `MOTOR=OFF`、`ECO=ON`，但文档和 Web 控制台统一使用上表命令。

## 8. 串口日志判断

Xshell 中正常日志：

```text
[NET] state topic=comfortfan/qihang01/state
[NET] command topic=comfortfan/qihang01/cmd
[NET] Wi-Fi link connected
[NET] DHCP ready
[NET] connecting MQTT=tcp://192.168.137.1:1883 client=qihang01
[NET] MQTT connected; subscribed=comfortfan/qihang01/cmd
[NET] RX comfortfan/qihang01/cmd => AUTO
[NET] TX {"device":"qihang01",...}
```

常见问题：

| 现象 | 优先检查 |
|------|----------|
| 一直没有 `Wi-Fi link connected` | Wi-Fi 名称、密码、开发板是否能连到该网络 |
| 有 Wi-Fi，没有 `DHCP ready` | 路由器或热点是否分配 IP |
| `MQTT connect failed` | `CF_MQTT_URI`、电脑 IP、防火墙、Mosquitto 是否启动 |
| 订阅不到状态 | 订阅主题是否和 `CF_DEVICE_ID` 一致 |
| 发布命令没反应 | 是否发到 `comfortfan/qihang01/cmd`，设备是否已 `MQTT connected` |

MQTT 通路正常后，再继续 [Web 控制台使用指南](./Web控制台使用指南.md)。
