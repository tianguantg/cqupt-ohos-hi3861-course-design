$ErrorActionPreference = "Stop"
Set-Location "C:\Program Files\mosquitto"
$HostIp = "127.0.0.1"
$Topic = "comfortfan/qihang01/cmd"
foreach ($cmd in @("QUIET", "AUTO", "ECO_ON", "STATUS", "MOTOR_ON", "MOTOR_OFF", "OFF")) {
  Write-Host "Sending $cmd"
  .\mosquitto_pub.exe -h $HostIp -p 1883 -t $Topic -m $cmd
  Start-Sleep -Seconds 2
}
