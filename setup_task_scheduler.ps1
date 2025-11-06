# Task Scheduler setup for Keyboard Audio Player (run as Administrator)
$exePath = Join-Path $PSScriptRoot "KeyboardAudioPlayer.exe"
Register-ScheduledTask -TaskName "KeyboardAudioPlayer" -Action (New-ScheduledTaskAction -Execute $exePath) -Trigger (New-ScheduledTaskTrigger -AtLogOn) -Principal (New-ScheduledTaskPrincipal -UserId $env:USERNAME -LogonType Interactive -RunLevel Highest) -Settings (New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -StartWhenAvailable) -Force
