@echo off
REM Helper script for managing Windows startup registration

echo ========================================
echo Keyboard Audio Player - Startup Manager
echo ========================================
echo.

echo This script helps manage the Windows startup registration.
echo.
echo Options:
echo   1. Register for startup (add to registry)
echo   2. Unregister from startup (remove from registry)
echo   3. Check startup status
echo   4. Exit
echo.

choice /c 1234 /n /m "Select option (1-4): "

if errorlevel 4 goto :exit
if errorlevel 3 goto :check
if errorlevel 2 goto :unregister
if errorlevel 1 goto :register

:register
echo.
echo Registering application for startup...
reg add "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v "KeyboardAudioPlayer" /t REG_SZ /d "%~dp0dist\KeyboardAudioPlayer.exe" /f
if errorlevel 1 (
    echo ERROR: Failed to register for startup
) else (
    echo SUCCESS: Application registered for startup
)
echo.
pause
goto :exit

:unregister
echo.
echo Unregistering application from startup...
reg delete "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v "KeyboardAudioPlayer" /f
if errorlevel 1 (
    echo ERROR: Failed to unregister (may not be registered)
) else (
    echo SUCCESS: Application unregistered from startup
)
echo.
pause
goto :exit

:check
echo.
echo Checking startup status...
reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v "KeyboardAudioPlayer" >nul 2>&1
if errorlevel 1 (
    echo STATUS: NOT registered for startup
) else (
    echo STATUS: Registered for startup
    echo.
    reg query "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v "KeyboardAudioPlayer"
)
echo.
pause
goto :exit

:exit
exit /b 0
