<#
.SYNOPSIS
    Sets up the Zephyr environment and builds/runs the application.

.DESCRIPTION
    This script sets up the Zephyr environment variables, builds the application,
    and optionally runs it in QEMU.

.PARAMETER BuildOnly
    If specified, only builds the application without running it.

.PARAMETER RunOnly
    If specified, only runs an existing binary without building it.

.PARAMETER BinaryPath
    Path to the binary file to run in QEMU. Required when RunOnly is specified.

.EXAMPLE
    # Build and run (default behavior)
    .\setup-env-and-build.ps1
    
.EXAMPLE
    # Only build the application
    .\setup-env-and-build.ps1 -BuildOnly
    
.EXAMPLE
    # Only run an existing binary
    .\setup-env-and-build.ps1 -RunOnly -BinaryPath "build\zephyr\zephyr.elf"
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

# Zephyr SDK configuration - Use the path from .env file
$ZephyrSdkPath = $env:ZEPHYR_SDK_INSTALL_DIR
if (-not $ZephyrSdkPath) {
    $ZephyrSdkPath = "C:\zephyr-sdk-0.17.0"
    Log "ZEPHYR_SDK_INSTALL_DIR not found in environment, using default: $ZephyrSdkPath" "Yellow"
}

# Load environment variables from .env file if it exists
$EnvFile = Join-Path $AppDir ".env"
if (Test-Path $EnvFile) {
    Log "Loading environment variables from .env file..." "Cyan"
    Get-Content $EnvFile | ForEach-Object {
        if ($_ -match '^\s*([^=]+)=(.*)$') {
            $key = $matches[1].Trim()
            $value = $matches[2].Trim()
            # Set environment variable
            [System.Environment]::SetEnvironmentVariable($key, $value, "Process")
            Log "Set environment variable: $key = $value" "Gray"
        }
    }
}

# Use ZEPHYR_BASE from environment (should be set from .env file)
$ZephyrBase = $env:ZEPHYR_BASE
if (-not $ZephyrBase) {
    $ZephyrBase = Join-Path $PSScriptRoot "..\..\tools\zephyr\zephyr"
    Log "ZEPHYR_BASE not found in environment, using default: $ZephyrBase" "Yellow"
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

# Function to set up Zephyr environment
function SetupZephyrEnvironment {
    Log "Setting up Zephyr environment..." "Cyan"
    
    # Check if Zephyr SDK exists
    if (-not (Test-Path $ZephyrSdkPath)) {
        Log "Zephyr SDK not found at: $ZephyrSdkPath" "Red"
        return $false
    }
    
    # Check if Zephyr base exists
    if (-not (Test-Path $ZephyrBase)) {
        Log "Zephyr base not found at: $ZephyrBase" "Red"
        Log "Please adjust the ZephyrBase variable in the script." "Red"
        return $false
    }
    
    # Set environment variables
    $env:ZEPHYR_SDK_INSTALL_DIR = $ZephyrSdkPath
    $env:ZEPHYR_BASE = $ZephyrBase
    
    Log "Zephyr environment set up successfully." "Green"
    Log "ZEPHYR_SDK_INSTALL_DIR: $env:ZEPHYR_SDK_INSTALL_DIR" "Green"
    Log "ZEPHYR_BASE: $env:ZEPHYR_BASE" "Green"
    
    return $true
}

# Function to build the application using CMake directly
function BuildApp {
    Log "Building application..." "Cyan"
    
    # Clean build directory
    if (Test-Path (Join-Path $AppDir "build")) {
        Log "Cleaning build directory..." "Cyan"
        Remove-Item -Path (Join-Path $AppDir "build") -Recurse -Force
    }
    
    # Create build directory
    New-Item -Path (Join-Path $AppDir "build") -ItemType Directory -Force | Out-Null
    
    # Change to build directory
    Push-Location (Join-Path $AppDir "build")
    
    # Run CMake with the correct board configuration
    # Use stm32h573i_dk board with QEMU emulation
    $cmakeCmd = "cmake -DBOARD=stm32h573i_dk -DCONF_FILE=prj.conf -DQEMU=1 .."
    $result = RunCommand -Command $cmakeCmd -Description "Running CMake"
    
    if (-not $result) {
        Pop-Location
        return $false
    }
    
    # Build the application
    $buildCmd = "cmake --build ."
    $result = RunCommand -Command $buildCmd -Description "Building application"
    
    # Return to original directory
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
Log "Starting environment setup and build at $(Get-Date)" "Cyan"
Log "Log file: $LogFile" "Cyan"

# Set up Zephyr environment
$envSetup = SetupZephyrEnvironment
if (-not $envSetup) {
    Log "Failed to set up Zephyr environment. Exiting." "Red"
    exit 1
}

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
