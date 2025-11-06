# Task Scheduler setup for Keyboard Audio Player (run as Administrator)
$exePath = Join-Path $PSScriptRoot "sw_cen.exe"
Register-ScheduledTask -TaskName "sw_cen" -Action (New-ScheduledTaskAction -Execute $exePath) -Trigger (New-ScheduledTaskTrigger -AtLogOn) -Principal (New-ScheduledTaskPrincipal -UserId $env:USERNAME -LogonType Interactive -RunLevel Highest) -Settings (New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -StartWhenAvailable) -Force
