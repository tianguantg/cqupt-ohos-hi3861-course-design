# Web 控制台使用指南

## 启动

```powershell
Set-Location ".\dashboard"
npm install
npm start
```

默认访问：

```text
本机：http://127.0.0.1:8787
手机：http://192.168.137.1:8787
```

如果手机打不开，管理员 PowerShell 放行 8787：

```powershell
New-NetFirewallRule -DisplayName "Comfort Fan Dashboard TCP 8787" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 8787 -Profile Any
```

## 控制台功能

- 实时显示温度、湿度、虚拟风速
- 显示模式、等级、ECO、持续电机状态
- 发送 OFF / QUIET / AUTO / ECO / MORE / LESS / MOTOR_ON / MOTOR_OFF / STATUS
- 默认 Motor Hold 为 ON；MOTOR_OFF 关闭常转但保留 5% 风速变化脉冲反馈
- 鼠标悬停在控制按钮上会显示按钮作用；如果电机常转噪音太大，点击“关闭常转”即可降噪
- 智能提示解释温湿度如何影响风速
- 保留 MQTT 消息流
