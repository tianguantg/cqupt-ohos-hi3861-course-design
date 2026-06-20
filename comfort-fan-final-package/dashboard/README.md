# Comfort Fan Dashboard / 舒适风扇控制台

这里放的是启航舒适风扇 v6 的本地远程控制界面，主要作为完整前端参考和故障对照。  
This is the local remote-control dashboard for the Qihang comfort fan v6, kept as a complete frontend reference.

## Speed Logic / 风速逻辑

The dashboard shows virtual speed, not continuous motor speed.  
界面显示的是虚拟风速，不是电机持续转速。

- OFF: target 0%.
- QUIET: target 25%, or 15% when ECO is enabled.
- AUTO: temperature and humidity jointly determine the target. Temperature first chooses the base target: `<26°C=0`, `26-27.4°C=30`, `27.5-28.9°C=45`, `29-30.9°C=60`, `>=31°C=75`.
- Humidity compensation is added on top of the temperature target: `>65% +5`, `>75% +10`.
- ECO applies a `-10` preference bias. MORE/LESS adjust the preference bias.
- The dashboard also shows a human-readable Comfort Insight message, such as detecting high temperature or humidity compensation and explaining why speed is increasing, decreasing, or staying stable.
- Smoothing limits virtual speed changes to 5% per sampling update, so speed may appear unchanged when the target is unchanged or while it is moving gradually.
- Levels: `0=OFF`, `1-35=LOW`, `36-60=MEDIUM`, `61+=HIGH`.
- The physical motor defaults to low-duty continuous hold. It also pulses on level changes or each 5% virtual-speed change; `MOTOR_OFF` disables continuous hold but keeps sensitive pulse feedback. If the motor is too noisy during a demo, hover the buttons for hints and click "关闭常转 / PULSE ONLY".

## Start / 启动

```powershell
Set-Location "C:\OpenHarmony\cqupt-ohos-hi3861-course-design\comfort-fan-final-package\dashboard"
npm install
npm start
```

Open / 打开：

```text
Local page / 本机页面：http://127.0.0.1:8787
LAN page / 局域网页面：http://192.168.137.1:8787
```

## Optional Environment Variables / 可选环境变量

```powershell
$env:PORT="8787"
$env:HOST="0.0.0.0"
$env:MQTT_URL="mqtt://192.168.137.1:1883"
$env:DEVICE_ID="qihang01"
npm start
```

## Commands / 支持命令

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
