# Simple Dashboard Template

这是给学生二次开发用的最小 Web 前端模板。它不替代 MQTT 测试，也不包含完整 dashboard 的所有功能。

建议使用方式：

```text
1. 先按通用配置步骤完成首次编译、烧录和串口验证
2. 再用 mosquitto_sub / mosquitto_pub 跑通 MQTT
3. 最后基于本目录编写自己的前端页面
```

## 文件

```text
index.html  页面结构和按钮
app.js      前端逻辑示例
```

## 运行

直接用浏览器打开：

```text
templates/simple-dashboard/index.html
```

本模板为了保持简单，没有直接连接 MQTT Broker。学生需要自行选择实现方式：

```text
方案 A：用 Node.js 写一个后端，把 HTTP 请求转成 MQTT publish
方案 B：使用支持 WebSocket 的 MQTT Broker，并在浏览器中接入 MQTT over WebSocket
方案 C：只把本模板作为页面原型，先用 mosquitto_pub 手动验证设备命令
```

完整参考实现保留在：

```text
comfort-fan-final-package/dashboard/
```

该目录用于学习参考和故障对照，不是课程主线的起点。
