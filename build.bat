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
    --name "KeyboardAudioPlayer" ^
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
echo Executable location: dist\KeyboardAudioPlayer.exe
echo.
echo To run the application:
echo   1. Double-click dist\KeyboardAudioPlayer.exe
echo   2. It will run in the background (no window)
echo   3. Press spacebar to play audio at max volume
echo   4. The app will auto-register for startup on first run
echo   5. To stop it, use Task Manager (Ctrl+Shift+Esc)
echo.
pause
