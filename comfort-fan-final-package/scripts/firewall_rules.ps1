New-NetFirewallRule -DisplayName "Comfort Fan MQTT TCP 1883" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 1883 -Profile Any -ErrorAction SilentlyContinue
New-NetFirewallRule -DisplayName "Comfort Fan Dashboard TCP 8787" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 8787 -Profile Any -ErrorAction SilentlyContinue
Get-NetFirewallRule -DisplayName "Comfort Fan*" | Format-Table DisplayName,Enabled,Profile,Direction,Action -AutoSize
