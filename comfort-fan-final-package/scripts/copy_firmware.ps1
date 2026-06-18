$ErrorActionPreference = "Stop"
$VmIp = "192.168.158.129"
$Remote = "/home/ohos/workspace/openharmony-3.2-release/out/qihang/qihang/Hi3861_wifiiot_app_allinone.bin"
$OutDir = "D:\03_Assets\Exchange\OpenHarmony\qihang"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
scp "ohos@${VmIp}:$Remote" $OutDir
Get-FileHash (Join-Path $OutDir "Hi3861_wifiiot_app_allinone.bin") -Algorithm SHA256

