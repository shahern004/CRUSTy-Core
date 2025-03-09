<#
.SYNOPSIS
    Simple script for building and running STM32H573I-DK applications in QEMU with logging.

.DESCRIPTION
    This script provides a simplified interface for building and running QEMU applications
    with built-in logging to ensure output is captured for visibility.

.PARAMETER BuildOnly
    If specified, only builds the application without running it.

.PARAMETER RunOnly
    If specified, only runs an existing binary without building it.

.PARAMETER BinaryPath
    Path to the binary file to run in QEMU. Required when RunOnly is specified.

.EXAMPLE
    # Build and run (default behavior)
    .\simple-qemu.ps1
    
.EXAMPLE
    # Only build the application
    .\simple-qemu.ps1 -BuildOnly
    
.EXAMPLE
    # Only run an existing binary
    .\simple-qemu.ps1 -RunOnly -BinaryPath "build\zephyr\zephyr.elf"
#>

param (
    [switch]$BuildOnly,
    [switch]$RunOnly,
    [string]$BinaryPath
)

# Script configuration
$ErrorActionPreference = "Stop"
$AppDir = $PSScriptRoot
$LogDir = Join-Path $AppDir "logs"
$Timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
$LogFile = Join-Path $LogDir "qemu-$Timestamp.log"
$DefaultBinaryPath = Join-Path $AppDir "build\zephyr\zephyr.elf"

# Create logs directory if it doesn't exist
if (-not (Test-Path $LogDir)) {
    New-Item -Path $LogDir -ItemType Directory -Force | Out-Null
    Write-Host "Created logs directory at: $LogDir" -ForegroundColor Green
}

# Initialize log file
"QEMU Log - $Timestamp" | Out-File -FilePath $LogFile -Encoding utf8
"----------------------------------------" | Out-File -FilePath $LogFile -Append -Encoding utf8

# Function to log messages
function Log {
    param (
        [string]$Message,
        [string]$Color = "White"
    )
    
    Write-Host $Message -ForegroundColor $Color
    $Message | Out-File -FilePath $LogFile -Append -Encoding utf8
}

# Function to run a command and log output
function RunCommand {
    param (
        [string]$Command,
        [string]$Description
    )
    
    Log "Running: $Description" "Cyan"
    Log "Command: $Command" "Gray"
    
    try {
        $output = Invoke-Expression $Command 2>&1
        $output | Out-File -FilePath $LogFile -Append -Encoding utf8
        Log "Command completed successfully." "Green"
        return $true
    }
    catch {
        Log "Command failed: $_" "Red"
        $_ | Out-File -FilePath $LogFile -Append -Encoding utf8
        return $false
    }
}

# Function to build the application
function BuildApp {
    Log "Building application..." "Cyan"
    
    # Clean build directory
    if (Test-Path (Join-Path $AppDir "build")) {
        Log "Cleaning build directory..." "Cyan"
        Remove-Item -Path (Join-Path $AppDir "build") -Recurse -Force
    }
    
    # Create build directory
    New-Item -Path (Join-Path $AppDir "build") -ItemType Directory -Force | Out-Null
    
    # Build command
    $buildCmd = "west build -b stm32h573i_dk --pristine -- -DCONF_FILE=prj.conf -DQEMU=1"
    
    # Run build in the app directory
    Push-Location $AppDir
    $result = RunCommand -Command $buildCmd -Description "Building application"
    Pop-Location
    
    if ($result) {
        Log "Build completed successfully." "Green"
        Log "Binary location: $DefaultBinaryPath" "Green"
    }
    else {
        Log "Build failed. See log for details." "Red"
    }
    
    return $result
}

# Function to run QEMU
function RunQemu {
    param (
        [string]$Binary
    )
    
    if (-not (Test-Path $Binary)) {
        Log "Binary not found: $Binary" "Red"
        return $false
    }
    
    Log "Running QEMU..." "Cyan"
    Log "Binary path: $Binary" "Cyan"
    
    # QEMU command
    $qemuCmd = "qemu-system-arm -cpu cortex-m33 -machine mps2-an500 -nographic -kernel `"$Binary`""
    
    # Run QEMU
    $result = RunCommand -Command $qemuCmd -Description "Running QEMU"
    
    if ($result) {
        Log "QEMU execution completed." "Green"
    }
    else {
        Log "QEMU execution failed. See log for details." "Red"
    }
    
    return $result
}

# Main execution logic
Log "Starting QEMU tool at $(Get-Date)" "Cyan"
Log "Log file: $LogFile" "Cyan"

$success = $true

if ($RunOnly) {
    # Run only mode
    if (-not $BinaryPath) {
        Log "Error: BinaryPath parameter is required when RunOnly is specified." "Red"
        exit 1
    }
    
    $success = RunQemu -Binary $BinaryPath
}
elseif ($BuildOnly) {
    # Build only mode
    $success = BuildApp
}
else {
    # Build and run mode
    $success = BuildApp
    
    if ($success) {
        $success = RunQemu -Binary $DefaultBinaryPath
    }
    else {
        Log "Build failed, skipping QEMU execution." "Red"
    }
}

# Final status
if ($success) {
    Log "Operation completed successfully." "Green"
    Log "Log file: $LogFile" "Cyan"
}
else {
    Log "Operation failed. See log file for details." "Red"
    Log "Log file: $LogFile" "Cyan"
    exit 1
}
