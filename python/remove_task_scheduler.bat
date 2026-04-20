@echo off
REM Remove Task Scheduler task for Keyboard Audio Player
REM This script must be run as Administrator

echo ========================================
echo Keyboard Audio Player - Remove Task
echo ========================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: This script must be run as Administrator!
    echo.
    echo Right-click this file and select "Run as administrator"
    echo.
    pause
    exit /b 1
)

echo Running with administrator privileges...
echo.

REM Check if task exists
schtasks /query /tn "sw_cen" >nul 2>&1
if %errorLevel% neq 0 (
    echo Task "sw_cen" does not exist.
    echo Nothing to remove.
    echo.
    pause
    exit /b 0
)

echo Removing scheduled task "sw_cen"...
echo.

REM Stop the task if it's running
echo Stopping task if running...
schtasks /end /tn "sw_cen" >nul 2>&1

REM Delete the task
schtasks /delete /tn "sw_cen" /f

if %errorLevel% neq 0 (
    echo.
    echo ERROR: Failed to remove scheduled task
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS! Task removed successfully.
echo ========================================
echo.
echo The application will no longer start automatically at login.
echo.
echo Note: If the application is currently running, you'll need to
echo stop it manually via Task Manager (Ctrl+Shift+Esc):
echo   Look for "sw_cen.exe" under Background processes
echo.
pause
