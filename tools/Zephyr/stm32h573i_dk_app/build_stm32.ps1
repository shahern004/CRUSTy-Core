# PRIMARY STM32 HARDWARE BUILD SCRIPT
# This is the canonical script for building the application for the STM32H573I-DK hardware
# Last Updated: 2025-03-09
#
# Usage: .\build_stm32.ps1
#
# This script will:
# 1. Configure the build for the STM32H573I-DK target
# 2. Build the application with hardware support enabled
# 3. Generate the binary files for flashing
# PowerShell script to build the application for STM32H573I-DK hardware

# Ensure west command is available
if (-not (Get-Command "west" -ErrorAction SilentlyContinue)) {
    Write-Host "Error: 'west' command not found. Please ensure Zephyr is properly installed." -ForegroundColor Red
    exit 1
}

# Set execution policy for this script
try {
    Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force
} catch {
    Write-Host "Warning: Could not set execution policy. You may need to run as administrator." -ForegroundColor Yellow
}

# Set the build directory
$BUILD_DIR = "build_stm32"

# Clean the build directory if requested
if ($args -contains "-c" -or $args -contains "--clean") {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Path $BUILD_DIR -Recurse -Force
    }
}

# Create build directory if it doesn't exist
if (-not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

# Get current working directory
$CURRENT_DIR = Get-Location

# Build the application for STM32H573I-DK
Write-Host "Building application for STM32H573I-DK hardware..." -ForegroundColor Cyan
$buildCommand = "west build -b stm32h573i_dk -d `"$BUILD_DIR`" --pristine -- -DCONF_FILE=`"prj.conf`" -DEXTRA_DTC_OVERLAY_FILE=`"boards/stm32h573i_dk.overlay`""

Write-Host "Executing: $buildCommand" -ForegroundColor DarkGray
Invoke-Expression $buildCommand

# Check build result
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Build successful" -ForegroundColor Green

# Ask if user wants to flash
Write-Host "Would you like to flash the application to the connected STM32H573I-DK board? (Y/N)" -ForegroundColor Yellow
$flash = Read-Host

if ($flash -eq "Y" -or $flash -eq "y") {
    Write-Host "Flashing application to STM32H573I-DK..." -ForegroundColor Cyan
    $flashCommand = "west flash -d $BUILD_DIR"
    
    Write-Host "Executing: $flashCommand" -ForegroundColor DarkGray
    Invoke-Expression $flashCommand
    
    # Check flash result
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Flash failed with exit code $LASTEXITCODE" -ForegroundColor Red
    } else {
        Write-Host "Flash successful" -ForegroundColor Green
    }
} else {
    Write-Host "Skipping flash" -ForegroundColor Yellow
    Write-Host "You can manually flash later with: west flash -d $BUILD_DIR" -ForegroundColor Cyan
}

# Return to original directory
Set-Location $CURRENT_DIR

