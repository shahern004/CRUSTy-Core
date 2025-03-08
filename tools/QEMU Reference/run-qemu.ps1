<#
.SYNOPSIS
    Build and run the STM32H573I-DK application in QEMU.

.DESCRIPTION
    This script builds the application for QEMU emulation using the qemu_cortex_m3 board target.
    This allows testing the application in QEMU without requiring the actual STM32H573I-DK hardware.

.PARAMETER Clean
    If specified, performs a clean build.

.PARAMETER Debug
    If specified, enables GDB debugging.

.PARAMETER Verbose
    If specified, enables verbose output for build and run commands.

.EXAMPLE
    .\run-qemu.ps1
    
.EXAMPLE
    .\run-qemu.ps1 -Clean -Debug -Verbose
#>

param (
    [switch]$Clean,
    [switch]$Debug,
    [switch]$Verbose
)

# Script configuration
$ErrorActionPreference = "Stop"
$AppDir = $PSScriptRoot
$ZephyrDir = "C:\zephyr-workspace\zephyr"

# Set environment variables
$env:ZEPHYR_BASE = $ZephyrDir

# Change to the application directory
Set-Location $AppDir

Write-Host "Building STM32H573I-DK application for QEMU..." -ForegroundColor Cyan

# Build options
$BuildOptions = @(
    "-b", "qemu_cortex_m3",
    "--pristine",
    "--", 
    "-DCONF_FILE=prj.conf",
    "-DQEMU=1"
)

# Build the application
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Cyan
    & west build -b qemu_cortex_m3 -t clean
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Clean failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
}

Write-Host "Running: west build $($BuildOptions -join ' ')" -ForegroundColor Gray
try {
    if ($Verbose) {
        & west build @BuildOptions --verbose
    } else {
        & west build @BuildOptions
    }
} catch {
    Write-Host "Build failed with error: $_" -ForegroundColor Red
    exit 1
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Build successful!" -ForegroundColor Green

# Run options
$RunOptions = @()

if ($Debug) {
    $RunOptions += @("--gdb", "--gdb-port", "1234", "--gdb-tcp")
    Write-Host "GDB server enabled on port 1234. Use 'target remote localhost:1234' in GDB to connect." -ForegroundColor Yellow
}

# Run the application in QEMU
Write-Host "Running application in QEMU..." -ForegroundColor Cyan
Write-Host "Press Ctrl+A, X to exit QEMU" -ForegroundColor Cyan

# Get the path to the built binary
$BinaryPath = Join-Path $AppDir "build\zephyr\zephyr.elf"

# QEMU command
$QemuCmd = "qemu-system-arm"
$QemuArgs = @(
    "-cpu", "cortex-m3",
    "-machine", "mps2-an385",
    "-nographic",
    "-kernel", $BinaryPath
)

if ($Debug) {
    $QemuArgs += @("-s", "-S")
    Write-Host "GDB server enabled on port 1234. Use 'target remote localhost:1234' in GDB to connect." -ForegroundColor Yellow
}

# Run QEMU
Write-Host "Running: $QemuCmd $($QemuArgs -join ' ')" -ForegroundColor Gray
try {
    & $QemuCmd @QemuArgs
} catch {
    Write-Host "QEMU failed with error: $_" -ForegroundColor Red
    exit 1
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "QEMU exited with code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "QEMU session ended." -ForegroundColor Green
