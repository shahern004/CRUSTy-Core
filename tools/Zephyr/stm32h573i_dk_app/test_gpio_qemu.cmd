@echo off
REM Windows batch file to build and test GPIO in QEMU
REM This uses absolute paths to avoid any path issues

SET APP_DIR=%~dp0
SET BUILD_DIR=%APP_DIR%\build_qemu
SET OVERLAY_FILE=%APP_DIR%\boards\stm32h573i_dk_qemu.overlay
SET CONFIG_FILE=%APP_DIR%\qemu.conf

echo Building and running STM32H573I-DK GPIO test in QEMU

REM Clean build directory if it exists
if exist "%BUILD_DIR%" (
    echo Cleaning build directory...
    rmdir /s /q "%BUILD_DIR%"
)

REM Create build directory
mkdir "%BUILD_DIR%"

REM Build with absolute paths
echo Building application...
west build -b stm32h573i_dk -d "%BUILD_DIR%" --pristine -- -DOVERLAY_CONFIG="%CONFIG_FILE%" -DEXTRA_DTC_OVERLAY_FILE="%OVERLAY_FILE%"

REM Check if build was successful
if %ERRORLEVEL% neq 0 (
    echo Build failed with exit code %ERRORLEVEL%
    exit /b %ERRORLEVEL%
)

echo Build successful!

REM Run in QEMU
echo Running in QEMU...
echo Press Ctrl+A X to exit QEMU
west build -t run -d "%BUILD_DIR%"

echo QEMU session ended
