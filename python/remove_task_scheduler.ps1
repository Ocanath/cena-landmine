# Remove Task Scheduler task for Keyboard Audio Player (run as Administrator)
Unregister-ScheduledTask -TaskName "sw_cen" -Confirm:$false
