$ErrorActionPreference = "Stop"
Stop-Service mosquitto -ErrorAction SilentlyContinue
Set-Location "C:\Program Files\mosquitto"
.\mosquitto.exe -c "C:\mosquitto-data\comfort-fan.conf" -v
