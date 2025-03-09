# CRUSTy-Core Development Todo List

## STM32H573I-DK QEMU Emulation Enhancement

### Phase 1: CPU Configuration Standardization

- ✅ Implement organic QEMU approach
  - Create simplified scripts for better reliability
  - Fix path issues with spaces in repository path
  - Resolve CONF_FILE parameter issues
  - Update documentation with detailed usage instructions
- ✅ Resolve CPU configuration discrepancies
  - Update Kconfig.defconfig to consistently use CPU_CORTEX_M33
  - Update board.cmake to use cortex-m33 CPU type
  - Update run-qemu.ps1 to use mps2-an500 machine for Cortex-M33

### Phase 2: Hardware Abstraction Layer

- ✅ Create dedicated HAL module
  - Implement hal.h and hal.c files with hardware-agnostic API
  - Add GPIO abstraction for both QEMU and real hardware
  - Add UART abstraction for both QEMU and real hardware
  - Implement runtime detection capabilities where possible

### Phase 2b: Zephyr STM32 HAL Integration (New Approach)

- ✅ Integrate Zephyr STM32 HAL
  - Moved Zephyr base from external location to tools/zephyr
  - Adapted to Zephyr conventions for API design
  - Used Zephyr's native mechanisms for environment detection
- ✅ Update build system for Zephyr integration
  - Configured CMakeLists.txt to include Zephyr STM32 HAL
  - Added Kconfig options for QEMU vs real hardware configuration
  - Updated scripts to use local Zephyr base
- ✅ Implement environment detection using Zephyr mechanisms
  - Used Kconfig-based configuration for high-level detection
  - Leveraged board-specific implementations for hardware details
  - Implemented conditional compilation for QEMU vs real hardware

### Phase 3: Application Enhancement with Zephyr HAL

- ✅ Adapt application code to Zephyr APIs
  - Refactored main application to use Zephyr HAL APIs
  - Added direct STM32 HAL access examples
  - Followed Zephyr coding conventions and patterns
- ⬜ Improve console output using Zephyr logging
  - Enhance logging with Zephyr's logging subsystem
  - Configure different log levels and categories
  - Add visual indicators for system status
- ⬜ Enhance LED functionality with Zephyr GPIO
  - Create LED patterns using Zephyr's GPIO APIs
  - Add visual console feedback for LED states
  - Implement demo mode with various LED effects
- ⬜ Add input simulation with Zephyr input subsystem
  - Implement button handling using Zephyr APIs
  - Create consistent interface across environments
  - Add interactive demos that respond to button presses
- ⬜ Implement timer-based demonstration
  - Use Zephyr's timer APIs for periodic tasks
  - Add visual feedback for timer events
  - Demonstrate timing consistency across platforms

### Phase 4: Zephyr Build System Integration

- ✅ Enhance Zephyr build configuration
  - Optimized CMake configuration for our specific needs
  - Configured conditional compilation using Kconfig
  - Created prj.conf with STM32 HAL configuration options
- ✅ Update QEMU scripts for Zephyr integration
  - Adapted scripts to work with local Zephyr base
  - Improved error handling and reporting
  - Updated environment variable handling

### Phase 5: Documentation

- ✅ Document architecture changes
  - Updated changelog.md with Zephyr STM32 HAL integration details
  - Updated context-summary.md with next steps
  - Documented the integration approach
- ⬜ Create peripheral compatibility chart
  - List available peripherals in QEMU vs real hardware
  - Document limitations and workarounds
- ⬜ Provide testing guidance
  - Create testing strategy for both environments
  - Document validation procedures

### Phase 6: Advanced Application Features

- ⬜ Implement advanced LED patterns
  - Create sophisticated LED patterns using PWM
  - Implement visual indicators for system status
  - Add demo mode with various LED effects
- ⬜ Create UART command interface
  - Implement command parsing for UART input
  - Add more sophisticated UART output formatting
  - Create a simple CLI for interacting with the application
- ⬜ Add sensor integration
  - Implement I2C communication for sensors
  - Add SPI communication for external devices
  - Create abstraction layer for sensor access
- ⬜ Implement timer-based features
  - Use Zephyr's timer APIs for periodic tasks
  - Create scheduling system for multiple periodic tasks
  - Add power management features

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
