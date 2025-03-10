# PRIMARY QEMU CLI TEST SCRIPT
# This is the canonical script for testing CLI functionality in QEMU
# It uses a minimal application designed specifically for reliable CLI testing
# Last Updated: 2025-03-09
#
# Usage: .\run_minimal_cli.ps1
# 
# This script will:
# 1. Build a minimal application with CLI support
# 2. Run it in QEMU
# 3. Allow testing of shell commands
#
# Commands to try:
# - help
# - crypto status
# - crypto random
# - hello
#
# Press Ctrl+A X to exit QEMU
# PowerShell script to run a minimal CLI test app in QEMU
# This uses a completely simplified application with only CLI functionality

# Paths to Zephyr workspace and minimal app
$CURRENT_DIR = Get-Location
$WORKSPACE_DIR = Join-Path -Path $CURRENT_DIR -ChildPath "tools/Zephyr"
$APP_DIR = Join-Path -Path $WORKSPACE_DIR -ChildPath "stm32h573i_dk_app/minimal_app"

# Check paths
if (-not (Test-Path $APP_DIR)) {
    Write-Error "Minimal app directory not found at: $APP_DIR"
    exit 1
}

# Convert paths to use forward slashes for CMake
$WORKSPACE_DIR = $WORKSPACE_DIR.Replace("\", "/")
$APP_DIR = $APP_DIR.Replace("\", "/")

# Set up build directory
$BUILD_DIR = "$WORKSPACE_DIR/build_minimal_cli"

# Clean the build directory
if (Test-Path $BUILD_DIR) {
    Write-Host "Cleaning old build directory..." -ForegroundColor Yellow
    Remove-Item -Path $BUILD_DIR -Recurse -Force
}
New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null

# Change directory to the Zephyr workspace
Write-Host "Changing to Zephyr workspace directory: $WORKSPACE_DIR" -ForegroundColor Cyan
Set-Location $WORKSPACE_DIR

try {
    # Build the minimal application for QEMU
    Write-Host "Building minimal CLI test app..." -ForegroundColor Green
    $buildCommand = "west build -b qemu_cortex_m3 -d `"$BUILD_DIR`" `"stm32h573i_dk_app/minimal_app`" --pristine"
    
    Write-Host "Executing: $buildCommand" -ForegroundColor DarkGray
    Invoke-Expression $buildCommand
    
    # Check if build succeeded
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    
    # Run the application with QEMU
    Write-Host "Running minimal CLI test app in QEMU..." -ForegroundColor Green
    Write-Host "----------------------------------------" -ForegroundColor Yellow
    Write-Host "Press Ctrl+A X to exit QEMU" -ForegroundColor Yellow
    Write-Host "Available test commands:" -ForegroundColor Green
    Write-Host "  help       - Show all available commands" -ForegroundColor Cyan
    Write-Host "  hello      - Simple hello world test" -ForegroundColor Cyan
    Write-Host "  crypto     - Test crypto commands" -ForegroundColor Cyan
    Write-Host "    status   - Show simulated crypto hardware status" -ForegroundColor Cyan 
    Write-Host "    random   - Generate random bytes" -ForegroundColor Cyan
    Write-Host "----------------------------------------" -ForegroundColor Yellow
    
    # Run QEMU and add a short delay before execution to make sure terminal is ready
    Start-Sleep -Seconds 1
    $runCommand = "west build -t run -d `"$BUILD_DIR`""
    Invoke-Expression $runCommand
    
} finally {
    # Return to the original directory
    Write-Host "QEMU session ended" -ForegroundColor Cyan
    Set-Location $CURRENT_DIR
}

