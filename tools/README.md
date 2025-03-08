# STM32H573I-DK QEMU Tools

Tools and reference files for STM32H573I-DK QEMU emulation.

## Contents

- **QEMU Reference/** - Reference implementation for STM32H573I-DK emulation
- **run-qemu.ps1** - Script for building and running in QEMU

## Quick Start

```powershell
cd C:\zephyr-workspace\stm32h573i_dk_app
.\run-qemu.ps1
```

## Debugging

```powershell
.\run-qemu.ps1 -Debug
```

Connect with GDB:

```
arm-none-eabi-gdb build/zephyr/zephyr.elf
(gdb) target remote localhost:1234
(gdb) continue
```

## Configuration Files

- `prj.conf` - Main project configuration
- `boards/qemu_cortex_m33.conf` - QEMU Cortex-M33 configuration
- `boards/stm32h573i_dk.overlay` - Device tree overlay

## Implementation Notes

- Uses mps2-an500 machine for Cortex-M33 emulation
- Limited peripheral emulation
- See [QEMU Unified Guide](../Documentation/QEMU_Unified_Guide.md) for details
