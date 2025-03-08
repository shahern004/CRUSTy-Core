<#
.SYNOPSIS
    Guides through manual installation and configures QEMU with STM32 support for emulating the STM32H573I-DK.

.DESCRIPTION
    This script guides you through the manual installation of QEMU with STM32 support for the STM32H573I-DK.
    It checks if QEMU is already installed, provides instructions for manual installation if needed,
    sets up necessary environment variables, and verifies the installation.
    
    The script is idempotent and can be run multiple times safely.

.PARAMETER QemuPath
    Path to the QEMU installation directory. If not provided, the script will attempt to find QEMU in common locations.

.NOTES
    File Name      : setup-qemu.ps1
    Prerequisite   : PowerShell 5.1 or later
                     Administrator privileges for system-wide configuration
    
.EXAMPLE
    .\setup-qemu.ps1
    
.EXAMPLE
    .\setup-qemu.ps1 -QemuPath "C:\Program Files\qemu"
#>

param (
    [string]$QemuPath
)

#Requires -Version 5.1

# Script configuration
$ErrorActionPreference = "Stop"

# Constants
$LogFile = Join-Path $PSScriptRoot "qemu-setup.log"
$DefaultQemuInstallDirs = @(
    "C:\Program Files\qemu",
    "C:\Program Files\qemu-stm32",
    "C:\Program Files (x86)\qemu",
    "C:\qemu"
)
$QemuTestFile = Join-Path $env:TEMP "qemu-test.bin"
$ManualInstallGuide = Join-Path $PSScriptRoot "qemu-manual-install.md"
$TempDir = Join-Path $env:TEMP "qemu-stm32-setup"

# Initialize log file
function Initialize-Log {
    if (Test-Path $LogFile) {
        # Keep the existing log but add a separator for this run
        Add-Content -Path $LogFile -Value "`n`n================ QEMU Setup $(Get-Date) ================`n"
    }
    else {
        # Create a new log file
        New-Item -Path $LogFile -ItemType File -Force | Out-Null
        Add-Content -Path $LogFile -Value "QEMU Setup Log - $(Get-Date)`n"
    }
}

# Write to log and console
function Write-Log {
    param (
        [Parameter(Mandatory=$true)]
        [string]$Message,
        
        [Parameter(Mandatory=$false)]
        [ValidateSet("INFO", "WARNING", "ERROR", "SUCCESS")]
        [string]$Level = "INFO"
    )
    
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $logMessage = "[$timestamp] [$Level] $Message"
    
    # Write to log file
    Add-Content -Path $LogFile -Value $logMessage
    
    # Write to console with color
    switch ($Level) {
        "INFO" { Write-Host $Message -ForegroundColor Cyan }
        "WARNING" { Write-Host $Message -ForegroundColor Yellow }
        "ERROR" { Write-Host $Message -ForegroundColor Red }
        "SUCCESS" { Write-Host $Message -ForegroundColor Green }
        default { Write-Host $Message }
    }
}

# Check if a command exists
function Test-Command {
    param (
        [Parameter(Mandatory=$true)]
        [string]$Command
    )
    
    $exists = $null -ne (Get-Command $Command -ErrorAction SilentlyContinue)
    return $exists
}

# Find QEMU installation
function Find-QemuInstallation {
    Write-Log "Looking for QEMU installation..."
    
    # If QemuPath parameter was provided, check that first
    if ($QemuPath) {
        if (Test-Path (Join-Path $QemuPath "bin\qemu-system-arm.exe")) {
            Write-Log "Found QEMU at specified path: $QemuPath" "SUCCESS"
            return $QemuPath
        }
        else {
            Write-Log "QEMU not found at specified path: $QemuPath" "WARNING"
        }
    }
    
    # Check if qemu-system-arm exists in PATH
    if (Test-Command "qemu-system-arm") {
        $qemuExePath = (Get-Command "qemu-system-arm").Source
        $qemuBinDir = Split-Path -Parent $qemuExePath
        $qemuInstallDir = Split-Path -Parent $qemuBinDir
        
        Write-Log "Found QEMU in PATH: $qemuInstallDir" "SUCCESS"
        return $qemuInstallDir
    }
    
    # Check common installation directories
    foreach ($dir in $DefaultQemuInstallDirs) {
        if (Test-Path (Join-Path $dir "bin\qemu-system-arm.exe")) {
            Write-Log "Found QEMU at: $dir" "SUCCESS"
            return $dir
        }
    }
    
    Write-Log "QEMU installation not found." "INFO"
    return $null
}

# Check if QEMU has STM32 support
function Test-QemuStm32Support {
    param (
        [Parameter(Mandatory=$true)]
        [string]$QemuInstallDir
    )
    
    Write-Log "Checking for STM32 support in QEMU..."
    
    $qemuExePath = Join-Path $QemuInstallDir "bin\qemu-system-arm.exe"
    
    try {
        # Get QEMU version
        $versionOutput = & $qemuExePath --version
        $versionMatch = $versionOutput -match "version\s+(\d+\.\d+\.\d+)"
        $installedVersion = $matches[1]
        
        Write-Log "Found QEMU version $installedVersion" "INFO"
        
        # Check available machines
        $machinesOutput = & $qemuExePath -M help
        $hasStm32Support = $machinesOutput -match "stm32"
        
        if ($hasStm32Support) {
            Write-Log "QEMU has STM32 support." "SUCCESS"
            return $true
        }
        else {
            Write-Log "QEMU does not have STM32 support." "INFO"
            Write-Log "Available machines: $($machinesOutput -join ', ')" "INFO"
            return $false
        }
    }
    catch {
        Write-Log "Error checking QEMU STM32 support: $_" "ERROR"
        return $false
    }
}

# Create manual installation guide
function Create-ManualInstallGuide {
    Write-Log "Creating manual installation guide..." "INFO"
    
    $guideContent = @"
# QEMU Manual Installation Guide for STM32H573I-DK Emulation

This guide will help you manually install QEMU with STM32 support for emulating the STM32H573I-DK.

## Option 1: Pre-built QEMU with STM32 Support

### Step 1: Download a Pre-built Package

Download one of these pre-built QEMU packages with STM32 support:

- [GNU MCU Eclipse QEMU](https://github.com/gnu-mcu-eclipse/qemu/releases)
  - This version has good support for STM32 boards
  - Download the latest Windows 64-bit installer

- [Zephyr SDK](https://github.com/zephyrproject-rtos/sdk-ng/releases)
  - Includes QEMU with STM32 support
  - Download the Windows 64-bit installer

### Step 2: Install the Package

1. Run the installer and follow the installation instructions
2. Make note of the installation directory (e.g., `C:\Program Files\qemu`)
3. Ensure the bin directory is added to your PATH

### Step 3: Verify the Installation

1. Open a new PowerShell window
2. Run: `qemu-system-arm --version`
3. Run: `qemu-system-arm -M help | findstr stm32`
4. If you see STM32 boards listed, the installation was successful

## Option 2: Build QEMU from Source

### Step 1: Install Prerequisites

Ensure you have the following installed:

- Git: [https://git-scm.com/download/win](https://git-scm.com/download/win)
- Python 3.x: [https://www.python.org/downloads/](https://www.python.org/downloads/)
- CMake: [https://cmake.org/download/](https://cmake.org/download/)
- Ninja: [https://github.com/ninja-build/ninja/releases](https://github.com/ninja-build/ninja/releases)
- Visual Studio 2019 or 2022 with "Desktop development with C++" workload

### Step 2: Clone the Repository

1. Open a PowerShell window
2. Run the following commands:

```powershell
# Create a directory for QEMU source
mkdir C:\qemu-source
cd C:\qemu-source

# Clone Antmicro's QEMU repository (good STM32 support)
git clone --branch stm32-upstream --depth 1 https://github.com/antmicro/qemu.git
cd qemu
```

### Step 3: Build QEMU

1. Open a "Developer Command Prompt for VS 2019" or "Developer Command Prompt for VS 2022"
2. Navigate to the QEMU source directory:

```cmd
cd C:\qemu-source\qemu
```

3. Create a build directory and configure the build:

```cmd
mkdir build
cd build
python ../configure --target-list=arm-softmmu --prefix="C:/Program Files/qemu-stm32" --disable-werror
```

4. Build and install QEMU:

```cmd
ninja
ninja install
```

### Step 4: Add QEMU to PATH

1. Open System Properties > Advanced > Environment Variables
2. Edit the PATH variable and add `C:\Program Files\qemu-stm32\bin`
3. Open a new PowerShell window and verify the installation:

```powershell
qemu-system-arm --version
qemu-system-arm -M help | findstr stm32
```

## Troubleshooting

### Common Issues

1. **Git clone fails**
   - Check your internet connection
   - Try using HTTPS instead of Git protocol
   - If behind a corporate firewall, configure Git to use your proxy

2. **Build fails**
   - Ensure all prerequisites are installed
   - Check that you're using the Developer Command Prompt
   - Try with a different branch or fork

3. **QEMU runs but no STM32 support**
   - Verify you're using a fork with STM32 support
   - Check the machine list with `qemu-system-arm -M help`
   - You may need to add a custom machine definition

## Next Steps

After installation, run the setup-qemu.ps1 script again with the path to your QEMU installation:

```powershell
.\setup-qemu.ps1 -QemuPath "C:\Program Files\qemu-stm32"
```
"@
    
    # Write the guide to a file
    $guideContent | Out-File -FilePath $ManualInstallGuide -Encoding UTF8
    
    Write-Log "Manual installation guide created at: $ManualInstallGuide" "SUCCESS"
    
    # Open the guide in the default text editor
    try {
        Start-Process $ManualInstallGuide
        Write-Log "Manual installation guide opened in default text editor." "INFO"
    }
    catch {
        Write-Log "Could not open manual installation guide: $_" "WARNING"
        Write-Log "Please open the guide manually at: $ManualInstallGuide" "WARNING"
    }
}

# Configure QEMU for STM32H573I-DK
function Configure-QemuForStm32 {
    param (
        [Parameter(Mandatory=$true)]
        [string]$QemuInstallDir
    )
    
    Write-Log "Configuring QEMU for STM32H573I-DK emulation..." "INFO"
    
    # Add QEMU to PATH if not already there
    $qemuBinDir = Join-Path $QemuInstallDir "bin"
    $currentPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
    
    if (-not $currentPath.Contains($qemuBinDir)) {
        try {
            Write-Log "Adding QEMU to user PATH..." "INFO"
            [System.Environment]::SetEnvironmentVariable("Path", "$currentPath;$qemuBinDir", "User")
            $env:Path = [System.Environment]::GetEnvironmentVariable("Path", "User") + ";" + [System.Environment]::GetEnvironmentVariable("Path", "Machine")
            Write-Log "QEMU added to user PATH." "SUCCESS"
        }
        catch {
            Write-Log "Failed to add QEMU to PATH: $_" "ERROR"
            Write-Log "Please add $qemuBinDir to your PATH manually." "WARNING"
        }
    }
    
    # Create a simple test for STM32 emulation
    Write-Log "Creating a simple test for STM32 emulation..." "INFO"
    
    # Create temp directory if it doesn't exist
    if (-not (Test-Path $TempDir)) {
        New-Item -Path $TempDir -ItemType Directory -Force | Out-Null
    }
    
    # Create a simple test file
    $testCode = @"
/* Simple test program for Cortex-M7 */
.syntax unified
.cpu cortex-m7
.thumb

.global _start
_start:
    /* Initialize stack pointer */
    ldr r0, =0x20010000
    mov sp, r0
    
    /* Simple loop */
    mov r0, #0
loop:
    add r0, r0, #1
    b loop
"@
    
    $testAsmFile = Join-Path $TempDir "test.s"
    $testCode | Out-File -FilePath $testAsmFile -Encoding ASCII
    
    # Create a linker script
    $linkerScript = @"
MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2M
    RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 1M
}

SECTIONS
{
    .text :
    {
        *(.text*)
    } > FLASH
    
    .data :
    {
        *(.data*)
    } > RAM
    
    .bss :
    {
        *(.bss*)
    } > RAM
}
"@
    
    $linkerScriptFile = Join-Path $TempDir "cortexm.ld"
    $linkerScript | Out-File -FilePath $linkerScriptFile -Encoding ASCII
    
    # Check if ARM GCC toolchain is installed
    $gccPath = $null
    $possiblePaths = @(
        "C:\Program Files (x86)\GNU Arm Embedded Toolchain\*\bin\arm-none-eabi-gcc.exe",
        "C:\Program Files\GNU Arm Embedded Toolchain\*\bin\arm-none-eabi-gcc.exe",
        "C:\Program Files\GNU Tools ARM Embedded\*\bin\arm-none-eabi-gcc.exe"
    )
    
    foreach ($path in $possiblePaths) {
        $gccPath = Get-ChildItem -Path $path -ErrorAction SilentlyContinue | Sort-Object -Property LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
        if ($gccPath) { break }
    }
    
    if (-not $gccPath) {
        Write-Log "ARM GCC toolchain not found. Skipping emulation test." "WARNING"
        Write-Log "Please install the ARM GCC toolchain to enable full testing." "WARNING"
        Write-Log "Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads" "INFO"
        return
    }
    
    # Compile the test program
    try {
        $gccDir = Split-Path -Parent $gccPath
        $objcopyPath = Join-Path $gccDir "arm-none-eabi-objcopy.exe"
        
        # Compile and link
        Write-Log "Compiling test program..." "INFO"
        & $gccPath -mcpu=cortex-m7 -mthumb -nostartfiles -T "$linkerScriptFile" -o "$TempDir\test.elf" $testAsmFile
        if ($LASTEXITCODE -ne 0) {
            Write-Log "Failed to compile test program." "WARNING"
            return
        }
        
        # Convert to binary
        & $objcopyPath -O binary "$TempDir\test.elf" $QemuTestFile
        if ($LASTEXITCODE -ne 0) {
            Write-Log "Failed to convert test program to binary." "WARNING"
            return
        }
        
        Write-Log "Test program compiled successfully." "SUCCESS"
    }
    catch {
        Write-Log "Error compiling test program: $_" "ERROR"
        return
    }
    
    # Create a QEMU launch script
    $launchScriptPath = Join-Path $PSScriptRoot "run-qemu.ps1"
    $launchScriptContent = @"
<#
.SYNOPSIS
    Runs QEMU emulation for STM32H573I-DK.

.DESCRIPTION
    This script launches QEMU with the appropriate settings for STM32H573I-DK emulation.
    It can be used to run ELF or binary files on the emulated STM32H573I-DK.

.PARAMETER BinaryFile
    Path to the binary or ELF file to run on the emulated STM32H573I-DK.

.PARAMETER Machine
    The QEMU machine type to use. Default is "stm32vldiscovery" which is compatible with STM32H5 series.

.PARAMETER Debug
    If specified, QEMU will be started with GDB server enabled on port 1234.

.PARAMETER Headless
    If specified, QEMU will run in headless mode (no GUI).

.EXAMPLE
    .\run-qemu.ps1 -BinaryFile firmware.bin
    
.EXAMPLE
    .\run-qemu.ps1 -BinaryFile firmware.elf -Debug
#>

param (
    [Parameter(Mandatory=`$true)]
    [string]`$BinaryFile,
    
    [Parameter(Mandatory=`$false)]
    [string]`$Machine = "stm32vldiscovery",
    
    [Parameter(Mandatory=`$false)]
    [switch]`$Debug,
    
    [Parameter(Mandatory=`$false)]
    [switch]`$Headless
)

# Check if the binary file exists
if (-not (Test-Path `$BinaryFile)) {
    Write-Error "Binary file not found: `$BinaryFile"
    exit 1
}

# Build the QEMU command
`$QemuArgs = @(
    "-M", `$Machine,
    "-kernel", "`"`$BinaryFile`""
)

# Add debug options if requested
if (`$Debug) {
    `$QemuArgs += @("-s", "-S")
    Write-Host "GDB server enabled on port 1234. Use 'target remote localhost:1234' in GDB to connect." -ForegroundColor Yellow
}

# Add headless options if requested
if (`$Headless) {
    `$QemuArgs += @("-nographic")
}

# Run QEMU
Write-Host "Starting QEMU with machine: `$Machine" -ForegroundColor Cyan
Write-Host "Press Ctrl+A, X to exit QEMU" -ForegroundColor Cyan
`$QemuArgsString = `$QemuArgs -join " "
Write-Host "qemu-system-arm `$QemuArgsString" -ForegroundColor Gray

& qemu-system-arm `$QemuArgs

# Check exit code
if (`$LASTEXITCODE -ne 0) {
    Write-Error "QEMU exited with code `$LASTEXITCODE"
    exit `$LASTEXITCODE
}
"@
    
    $launchScriptContent | Out-File -FilePath $launchScriptPath -Encoding UTF8
    Write-Log "QEMU launch script created at: $launchScriptPath" "SUCCESS"
    
    # Test the emulation
    try {
        Write-Log "Testing STM32 emulation..." "INFO"
        $qemuExePath = Join-Path $qemuBinDir "qemu-system-arm.exe"
        
        # Run the test with the launch script
        Write-Log "Running test program in QEMU..." "INFO"
        Write-Log "This will open a new window. Close it after a few seconds to continue." "INFO"
        
        Start-Process -FilePath "powershell.exe" -ArgumentList "-ExecutionPolicy Bypass -File `"$launchScriptPath`" -BinaryFile `"$QemuTestFile`" -Headless" -Wait
        
        Write-Log "STM32 emulation test completed." "SUCCESS"
    }
    catch {
        Write-Log "Error testing STM32 emulation: $_" "ERROR"
    }
}

# Clean up temporary files
function Remove-TempFiles {
    if (Test-Path $TempDir) {
        Write-Log "Cleaning up temporary files..." "INFO"
        try {
            Remove-Item -Path $TempDir -Recurse -Force -ErrorAction SilentlyContinue
            Write-Log "Temporary files cleaned up." "SUCCESS"
        }
        catch {
            Write-Log "Failed to clean up temporary files: $_" "WARNING"
        }
    }
}

# Main function
function Main {
    try {
        Initialize-Log
        Write-Log "Starting QEMU setup for STM32H573I-DK emulation..." "INFO"
        
        # Find QEMU installation
        $qemuInstallDir = Find-QemuInstallation
        
        if ($qemuInstallDir) {
            # Check if QEMU has STM32 support
            $hasStm32Support = Test-QemuStm32Support -QemuInstallDir $qemuInstallDir
            
            if ($hasStm32Support) {
                Write-Log "QEMU with STM32 support is already installed at: $qemuInstallDir" "SUCCESS"
                
                # Configure QEMU for STM32H573I-DK
                Configure-QemuForStm32 -QemuInstallDir $qemuInstallDir
            }
            else {
                Write-Log "QEMU is installed but does not have STM32 support." "WARNING"
                Write-Log "Please install a version of QEMU with STM32 support." "INFO"
                
                # Create manual installation guide
                Create-ManualInstallGuide
                
                Write-Log "After installing QEMU with STM32 support, run this script again with the -QemuPath parameter:" "INFO"
                Write-Log ".\setup-qemu.ps1 -QemuPath ""C:\Path\To\QEMU""" "INFO"
            }
        }
        else {
            Write-Log "QEMU is not installed." "INFO"
            
            # Create manual installation guide
            Create-ManualInstallGuide
            
            Write-Log "After installing QEMU with STM32 support, run this script again with the -QemuPath parameter:" "INFO"
            Write-Log ".\setup-qemu.ps1 -QemuPath ""C:\Path\To\QEMU""" "INFO"
        }
        
        # Final success message
        Write-Log "QEMU setup script completed." "SUCCESS"
        Write-Log "For more information, see the documentation in Documentation/Guides/QEMU_EMULATION.md" "INFO"
        
        # Clean up
        Remove-TempFiles
    }
    catch {
        Write-Log "Error during QEMU setup: $_" "ERROR"
        Write-Log "Setup failed. Please check the log file at $LogFile for details." "ERROR"
        
        # Clean up
        Remove-TempFiles
        
        # Return non-zero exit code
        exit 1
    }
}

# Run the main function
Main
