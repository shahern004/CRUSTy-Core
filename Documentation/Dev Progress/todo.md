# CRUSTy-Core Development Todo List

## STM32H573I-DK QEMU Emulation Enhancement

### Phase 1: CPU Configuration Standardization

- ⬜ Resolve CPU configuration discrepancies
  - Update Kconfig.defconfig to consistently use CPU_CORTEX_M33
  - Update board.cmake to use cortex-m33 CPU type
  - Update run-qemu.ps1 to use mps2-an500 machine for Cortex-M33

### Phase 2: Hardware Abstraction Layer

- ⬜ Create dedicated HAL module
  - Implement hal.h and hal.c files with hardware-agnostic API
  - Add GPIO abstraction for both QEMU and real hardware
  - Add UART abstraction for both QEMU and real hardware
  - Implement runtime detection capabilities where possible

### Phase 3: Application Enhancement

- ⬜ Improve console output
  - Add more detailed logging with different log levels
  - Implement visual indicators for system status
- ⬜ Enhance LED functionality
  - Create more sophisticated LED patterns
  - Add visual console feedback for LED states
- ⬜ Add input simulation
  - Implement button input simulation in QEMU
  - Create consistent API for button handling
- ⬜ Implement timer-based demonstration
  - Add periodic tasks with visual feedback
  - Demonstrate timing consistency across platforms

### Phase 4: Dual-Target Build System

- ⬜ Enhance build system
  - Update CMakeLists.txt for better dual-target support
  - Improve conditional compilation structure
  - Create helper scripts for building and testing
- ⬜ Update run-qemu.ps1
  - Add support for enhanced features
  - Improve error handling and reporting
  - Add options for different test scenarios

### Phase 5: Documentation

- ⬜ Document architecture differences
  - Create comparison of Cortex-M3 vs Cortex-M33
  - Document memory map differences
  - Explain performance implications
- ⬜ Create peripheral compatibility chart
  - List available peripherals in QEMU vs real hardware
  - Document limitations and workarounds
- ⬜ Provide testing guidance
  - Create testing strategy for both environments
  - Document validation procedures

## STM32H573I-DK Migration

### Phase 1: Embedded Build Setup

- ✅ Create CMake toolchain file for STM32H573I-DK target
- ✅ Test ARM toolchain configuration
- ⬜ Update Rust crypto library's configuration for ARM Cortex-M7 target
  - Add feature flags for conditional compilation
  - Configure dependencies to work without standard library
  - Add embedded-specific dependencies
- ⬜ Update CMakeLists.txt to support building for embedded target
  - Add option for building embedded target
  - Configure STM32CubeH5 HAL library
  - Add appropriate definitions for embedded target
  - Set up Rust flags for embedded target
- ⬜ Add CMake preset for STM32H573I-DK target
  - Create "stm32h573i-dk" preset in CMakePresets.json
  - Configure to use ARM toolchain file
  - Enable BUILD_EMBEDDED_TARGET option
  - Disable USE_QT option

### Phase 2: Hardware Abstraction Layer

- ⬜ Create embedded main entry point
- ⬜ Implement hardware abstraction layer
- ⬜ Configure memory protection

### Phase 3: Communication Protocol

- ⬜ Define communication protocol
- ⬜ Implement communication protocol
- ⬜ Implement command handlers

### Phase 4: Hardware Acceleration

- ⬜ Adapt Rust crypto library for hardware acceleration
- ⬜ Implement C++ hardware acceleration interface

### Phase 5: PC-Side Integration

- ⬜ Create PC-side communication interface
- ⬜ Implement serial communication
- ⬜ Modify Encryptor class for embedded device support

### Phase 6: Testing & Optimization

- ⬜ Create integration tests
- ⬜ Optimize performance
- ⬜ Validate security
