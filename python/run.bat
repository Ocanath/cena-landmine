@echo off
echo Starting Keyboard Audio Player...
echo.

REM Check if venv exists
if not exist "venv\Scripts\python.exe" (
    echo ERROR: Virtual environment not found!
    echo Please run build.bat first to set up the environment.
    pause
    exit /b 1
)

REM Run the Python script
venv\Scripts\python.exe keyboard_player.py

REM Capture exit code
set EXIT_CODE=%ERRORLEVEL%

echo.
echo.
echo Script exited with code: %EXIT_CODE%

if %EXIT_CODE% neq 0 (
    echo.
    echo ERROR: Script encountered an error.
    echo Check the output above for details.
)

echo.
pause