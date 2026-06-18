# Web 控制台参考实现说明

本目录是完整 dashboard 参考实现，用于学习、对照和故障排查。课程主线建议先完成 MQTT 命令行验证，再基于 [simple-dashboard 模板](../../templates/simple-dashboard/README.md) 编写自己的前端。

完整 dashboard 不是第一次联调的必需步骤。它不是直接连接开发板，而是连接 Windows 上的 Mosquitto Broker。

## 1. 运行关系

```text
浏览器
  ↓ HTTP / WebSocket
dashboard/server.js
  ↓ MQTT
Mosquitto Broker
  ↓ MQTT
Hi3861 开发板 fan_manager.c
```

设备端先按 [编译烧录指南](./编译烧录指南.md) 烧录成功，再按 [MQTT 测试指南](./MQTT测试指南.md) 确认 MQTT 通路正常。需要参考完整实现时，再阅读本文。

## 2. Web 代码结构

```text
comfort-fan-final-package/dashboard/
├── package.json
├── server.js
└── public/
    ├── index.html
    ├── app.js
    └── styles.css
```

| 文件 | 作用 |
|------|------|
| `server.js` | Node.js 后端，连接 MQTT，提供网页和 WebSocket，转发控制命令 |
| `public/index.html` | 页面结构、按钮、按钮 tooltip |
| `public/app.js` | 前端状态渲染、WebSocket 接收、按钮点击请求 |
| `public/styles.css` | 页面样式 |
| `package.json` | 依赖和 `npm start` 命令 |

## 3. 后端默认配置

`server.js` 默认值：

```js
const PORT = Number(process.env.PORT || 8787);
const HOST = process.env.HOST || "0.0.0.0";
const MQTT_URL = process.env.MQTT_URL || "mqtt://192.168.137.1:1883";
const DEVICE_ID = process.env.DEVICE_ID || "qihang01";
```

默认主题：

```text
comfortfan/qihang01/state
comfortfan/qihang01/cmd
comfortfan/qihang01/event
```

这些主题必须和设备端 `network_config.h` 中的 `CF_DEVICE_ID` 一致。

## 4. 启动 Mosquitto

Windows PowerShell：

```powershell
Set-Location "C:\Program Files\mosquitto"
.\mosquitto.exe -v
```

如果还没有放行端口：

```powershell
New-NetFirewallRule -DisplayName "Mosquitto MQTT TCP 1883" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1883 -Profile Any
```

## 5. 启动完整参考实现

Windows PowerShell：

```powershell
Set-Location "C:\OpenHarmony\cqupt-ohos-hi3861-course-design\comfort-fan-final-package\dashboard"
npm install
npm start
```

默认访问：

```text
本机：http://127.0.0.1:8787
局域网：http://电脑IP:8787
```

如果浏览器打不开，放行 8787：

```powershell
New-NetFirewallRule -DisplayName "Comfort Fan Dashboard TCP 8787" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 8787 -Profile Any
```

## 6. 根据网络环境修改启动参数

如果 Mosquitto 不在 `192.168.137.1`，启动前设置环境变量：

```powershell
$env:MQTT_URL="mqtt://电脑IP地址:1883"
$env:DEVICE_ID="qihang01"
$env:PORT="8787"
npm start
```

示例：电脑局域网 IP 是 `192.168.1.23`：

```powershell
$env:MQTT_URL="mqtt://192.168.1.23:1883"
npm start
```

如果设备端改成：

```c
#define CF_DEVICE_ID "team01"
```

Web 控制台也要使用：

```powershell
$env:DEVICE_ID="team01"
npm start
```

## 7. 按钮和命令对应关系

Web 按钮最终会调用：

```text
POST /api/command
```

后端 `server.js` 会把命令发布到：

```text
comfortfan/qihang01/cmd
```

支持命令：

| 命令 | 按钮作用 |
|------|----------|
| `OFF` | 关闭风扇 |
| `QUIET` | 安静模式 |
| `AUTO` | 自动模式 |
| `ECO_ON` | 开启 ECO |
| `ECO_OFF` | 关闭 ECO |
| `MORE` | 提高舒适偏置 |
| `LESS` | 降低舒适偏置 |
| `MOTOR_ON` | 开启低功率常转 |
| `MOTOR_OFF` | 关闭常转，仅保留脉冲反馈 |
| `STATUS` | 请求设备立即上报状态 |

鼠标悬停在按钮上会显示按钮作用。如果电机常转噪音太大，点击“关闭常转 / PULSE ONLY”。

## 8. 状态数据显示来源

设备端每秒向 `state` 主题发布 JSON。Web 后端订阅：

```js
client.subscribe([topics.state, topics.event], { qos: 0 });
```

前端显示字段：

| 页面显示 | MQTT JSON 字段 |
|----------|----------------|
| 温度 | `temperature` |
| 湿度 | `humidity` |
| 虚拟风速 | `virtual_speed` |
| 模式 | `mode` |
| 档位 | `level` |
| ECO | `eco` |
| Motor Hold | `motor_hold` |
| 设备在线 | `wifi` 和 `mqtt` |

如果页面显示 `Broker offline`，说明 Web 后端没有连上 Mosquitto。  
如果页面 Broker 在线但设备状态不更新，说明设备没有向对应 `state` 主题上报。

## 9. 修改前端时看哪里

常见修改位置：

| 想修改 | 文件 |
|--------|------|
| 按钮文字、tooltip、布局结构 | `public/index.html` |
| 按钮点击后发送什么命令 | `public/index.html` 的 `data-command` |
| 状态字段如何显示 | `public/app.js` |
| 温湿度解释文字 | `public/app.js` |
| 页面颜色和布局 | `public/styles.css` |
| MQTT 地址、设备 ID、端口 | `server.js` 或启动前设置环境变量 |

修改后刷新浏览器即可。修改 `server.js` 后需要停止并重新执行：

```powershell
npm start
```

## 10. 常见问题

| 现象 | 处理 |
|------|------|
| `npm` 不是命令 | 安装 Node.js 后重新打开 PowerShell |
| `Broker offline` | 检查 Mosquitto 是否启动、`MQTT_URL` 是否正确 |
| 页面能打开但没有设备数据 | 先用 `mosquitto_sub` 订阅 `comfortfan/qihang01/#` |
| 手机打不开页面 | 确认电脑和手机同一网络，放行 8787 |
| 按钮没效果 | 检查命令主题和 `DEVICE_ID` 是否与设备端一致 |
