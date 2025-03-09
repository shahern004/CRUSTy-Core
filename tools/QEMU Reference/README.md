# QEMU Reference

This directory contains reference files for STM32H573I-DK QEMU emulation. All necessary files for QEMU emulation are included in this directory, making it self-contained within the repository.

## Prerequisites

Before using the QEMU Reference, ensure you have:

1. **Zephyr SDK** installed (tested with version 0.17.0)
2. **QEMU** with ARM support installed
3. **West** build tool installed

## Usage: Organic QEMU Approach

We've implemented a simplified "organic" approach to QEMU emulation that avoids complex path manipulation and provides more reliable operation. This approach separates the build and run processes for better control.

### Building the Application

To build the application without running it:

```powershell
# Navigate to the QEMU Reference directory
cd tools\QEMU Reference

# Build with default settings
.\build-app.ps1

# For a clean build
.\build-app.ps1 -Clean

# For verbose output
.\build-app.ps1 -Verbose
```

### Running in QEMU

To run an existing binary in QEMU:

```powershell
# Run the default binary
.\run-qemu-only.ps1 -BinaryPath "build\zephyr\zephyr.elf"

# For debugging support
.\run-qemu-only.ps1 -BinaryPath "build\zephyr\zephyr.elf" -Debug

# For verbose output
.\run-qemu-only.ps1 -BinaryPath "build\zephyr\zephyr.elf" -Verbose
```

### Combined Build and Run

To build and run in one step:

```powershell
# Build and run with default settings
.\run-qemu-direct.ps1

# For debugging support
.\run-qemu-direct.ps1 -Debug

# For verbose output
.\run-qemu-direct.ps1 -Verbose

# To specify a custom binary path
.\run-qemu-direct.ps1 -BinaryPath "path\to\your\binary.elf"
```

## How It Works

The organic QEMU approach:

1. Uses direct QEMU commands without complex path manipulation
2. Separates building and running for better control
3. Avoids issues with spaces in paths
4. Provides clearer error messages and better reliability

## Directory Structure

- `main.c` - Main application source code
- `prj.conf` - Project configuration
- `boards/` - Board configuration files
- `src/` - Additional source files
- `build-app.ps1` - Script to build the application
- `run-qemu-only.ps1` - Script to run an existing binary in QEMU
- `run-qemu-direct.ps1` - Script to build and run in one step
- `setup-qemu.ps1` - Script to set up QEMU for STM32H573I-DK emulation
- `run-qemu.ps1` - Legacy script (kept for reference)

## Direct QEMU Commands

For those who prefer to run QEMU directly, you can use these commands:

```powershell
# Build the application
west build -b qemu_cortex_m3 --pristine -- -DCONF_FILE="prj.conf" -DQEMU=1

# Run in QEMU
qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -kernel "build\zephyr\zephyr.elf"

# Run with debugging enabled
qemu-system-arm -cpu cortex-m3 -machine mps2-an385 -nographic -kernel "build\zephyr\zephyr.elf" -s -S
```

## Troubleshooting

If you encounter build errors:

1. Ensure your Zephyr SDK path is correct in the `.env` file
2. Try running with the `-Clean` parameter to start fresh
3. Check that QEMU is properly installed and in your PATH
4. Run with `-Verbose` to see detailed output

For detailed documentation, see:

- [Main Tools README](../README.md)
- [QEMU Guide](../../Documentation/Embedded/QEMU_GUIDE.md)
