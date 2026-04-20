@echo off
REM Build script for creating standalone executable

echo ========================================
echo Building Keyboard Audio Player
echo ========================================
echo.

REM Check if venv exists, create if not
if not exist "venv\" (
    echo Creating virtual environment...
    python -m venv venv
    if errorlevel 1 (
        echo ERROR: Failed to create virtual environment
        pause
        exit /b 1
    )
    echo Virtual environment created.
    echo.
)

REM Activate virtual environment
echo Activating virtual environment...
call venv\Scripts\activate.bat
if errorlevel 1 (
    echo ERROR: Failed to activate virtual environment
    pause
    exit /b 1
)

REM Install/update dependencies
echo.
echo Installing dependencies from requirements.txt...
pip install -r requirements.txt
if errorlevel 1 (
    echo ERROR: Failed to install dependencies
    pause
    exit /b 1
)

REM Clean previous build artifacts
echo.
echo Cleaning previous build artifacts...
if exist "build\" rmdir /s /q build
if exist "dist\" rmdir /s /q dist
if exist "*.spec" del /q *.spec

REM Build executable with PyInstaller
echo.
echo Building executable with PyInstaller...
pyinstaller --onefile ^
    --noconsole ^
    --add-data "sounds/notification.wav;sounds" ^
    --name "sw_cen" ^
    keyboard_player.py

if errorlevel 1 (
    echo ERROR: PyInstaller build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: dist\sw_cen.exe
echo.
echo NEXT STEPS:
echo.
echo 1. SET UP AUTO-START (One-time, requires admin):
echo    Right-click setup_task_scheduler.bat and "Run as administrator"
echo    This creates a Task Scheduler task to run at login with admin privileges.
echo    You'll only be prompted ONCE during setup (not on every startup).
echo.
echo 2. TEST THE APPLICATION:
echo    After setup, run: schtasks /run /tn "sw_cen"
echo    Or reboot and it will start automatically.
echo.
echo 3. HOW IT WORKS:
echo    - Runs silently in the background (no window)
echo    - Press spacebar anywhere to play audio at max volume
echo    - To stop: Use Task Manager (Ctrl+Shift+Esc) and end "sw_cen.exe"
echo.
echo 4. TO UNINSTALL:
echo    Right-click remove_task_scheduler.bat and "Run as administrator"
echo.
pause
