# STM32H573I-DK QEMU Tools

Tools and reference files for STM32H573I-DK QEMU emulation.

## Contents

- **QEMU Reference/** - Self-contained reference implementation for STM32H573I-DK emulation
  - Contains all necessary files for QEMU emulation of the STM32H573I-DK board
  - Includes build configuration, board definitions, and run scripts
  - Works directly from the repository without external dependencies (except Zephyr SDK)

## Quick Start: Organic QEMU Approach

We've implemented a simplified "organic" approach to QEMU emulation that avoids complex path manipulation and provides more reliable operation.

```powershell
# Navigate to the QEMU Reference directory
cd tools\QEMU Reference

# Build and run in one step
.\run-qemu-direct.ps1

# Or build and run separately
.\build-app.ps1
.\run-qemu-only.ps1 -BinaryPath "build\zephyr\zephyr.elf"
```

## Features

- **Self-contained** - All necessary files are included in the repository
- **Simplified approach** - Direct QEMU commands without complex path manipulation
- **Separated concerns** - Build and run processes can be performed separately
- **Debugging support** - Includes GDB server integration
- **Clean build option** - Start fresh with `-Clean` parameter

## Zephyr SDK Configuration

The scripts require the Zephyr SDK. You can specify the Zephyr SDK directory in one of these ways:

1. Set the ZEPHYR_BASE environment variable
2. Create a `.env` file in the QEMU Reference directory with the following content:
   ```
   ZEPHYR_BASE=C:/path/to/zephyr
   ZEPHYR_TOOLCHAIN_VARIANT=zephyr
   ZEPHYR_SDK_INSTALL_DIR=C:/path/to/zephyr-sdk
   ```

## Implementation Notes

- Uses mps2-an385 machine for Cortex-M3 emulation
- Limited peripheral emulation
- Avoids issues with spaces in paths
- See [QEMU Guide](../Documentation/Embedded/QEMU_GUIDE.md) for detailed documentation
- See [QEMU Reference README](QEMU%20Reference/README.md) for detailed usage instructions
