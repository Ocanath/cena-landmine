@echo off
REM Task Scheduler setup for Keyboard Audio Player
REM This script must be run as Administrator (one-time setup)

echo ========================================
echo Keyboard Audio Player - Task Scheduler Setup
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

REM Check if executable exists
if not exist "dist\KeyboardAudioPlayer.exe" (
    echo ERROR: KeyboardAudioPlayer.exe not found!
    echo.
    echo Please run build.bat first to create the executable.
    echo.
    pause
    exit /b 1
)

REM Get the full path to the executable
set "EXE_PATH=%~dp0dist\KeyboardAudioPlayer.exe"

echo Creating Task Scheduler task...
echo.
echo Task name: KeyboardAudioPlayer
echo Executable: %EXE_PATH%
echo Trigger: At user logon
echo Privileges: Run with highest privileges (admin)
echo.

REM Create the scheduled task
schtasks /create ^
    /tn "KeyboardAudioPlayer" ^
    /tr "\"%EXE_PATH%\"" ^
    /sc onlogon ^
    /rl highest ^
    /f

if %errorLevel% neq 0 (
    echo.
    echo ERROR: Failed to create scheduled task
    pause
    exit /b 1
)

echo.
echo ========================================
echo SUCCESS! Task created successfully.
echo ========================================
echo.
echo The application will now start automatically when you log in.
echo It will run silently in the background with admin privileges.
echo.
echo To start it now without rebooting, you can either:
echo   1. Run: schtasks /run /tn "KeyboardAudioPlayer"
echo   2. Or double-click dist\KeyboardAudioPlayer.exe
echo.
echo To remove the task later, run:
echo   schtasks /delete /tn "KeyboardAudioPlayer" /f
echo.
pause
