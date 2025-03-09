# CRUSTy-Core Development Changelog

## 2025-03-08 (Latest)

- Improved terminal output visibility and script organization
  - Created dedicated "logs" folder in QEMU Reference directory for consistent log file storage
  - Consolidated multiple QEMU scripts into two simplified scripts:
    - `simple-qemu.ps1`: Comprehensive script for QEMU operations with built-in logging
    - `hardware-tool.ps1`: Placeholder script for future hardware operations with consistent interface
  - Fixed issues with terminal output visibility in Act mode
  - Implemented robust error handling and logging in all scripts
  - Standardized parameter naming to avoid conflicts
  - Test results: Successfully created and verified log file creation and output capture

## 2025-03-08

- Standardized CPU configuration for STM32H573I-DK QEMU emulation

  - Updated Kconfig.defconfig to consistently use CPU_CORTEX_M33 instead of CPU_CORTEX_M7
  - Updated board.cmake to use cortex-m33 CPU type
  - Updated all QEMU scripts to use cortex-m33 CPU and mps2-an500 machine
  - Fixed board target name from qemu_stm32h573i_dk to stm32h573i_dk
  - Ensured consistent configuration across all build and run scripts
  - Test results: Successfully built and ran the application with the corrected configuration

- Implemented terminal output visibility workaround for Cline

  - Created `tools/command-wrapper.ps1` to capture command output to files
  - Developed enhanced logging versions of QEMU scripts (`build-app-with-logging.ps1` and `run-qemu-only-with-logging.ps1`)
  - Added comprehensive documentation in `Documentation/Dev Progress/terminal-output-workaround.md`
  - Implemented best practices for ensuring command output visibility in Cline
  - Test results: Successfully demonstrated workarounds for terminal output visibility issues in Cline

- Implemented organic QEMU approach for STM32H573I-DK emulation
  - Created simplified scripts for better reliability and maintainability
  - Developed `build-app.ps1` for focused application building
  - Created `run-qemu-only.ps1` for running existing binaries in QEMU
  - Implemented `run-qemu-direct.ps1` for combined build and run functionality
  - Fixed path issues with spaces in repository path
  - Resolved CONF_FILE parameter issues by properly quoting parameters
  - Updated documentation with detailed usage instructions and examples
  - Added direct QEMU command examples for manual execution
  - Test results: Scripts successfully build and run the application in QEMU without path-related issues

## 2025-03-07

- Streamlined project documentation structure

  - Created central Documentation/README.md as a unified documentation index
  - Consolidated QEMU documentation into a comprehensive QEMU_Unified_Guide.md
  - Created unified Embedded_Unified_Guide.md combining STM32H573I-DK migration and embedded integration
  - Implemented development context templates for different types of tasks
  - Added documentation status tracking to improve visibility
  - Test results: Documentation is now better organized, more accessible, and provides clearer context

- Developed detailed enhancement plan for STM32H573I-DK QEMU emulation

  - Analyzed current implementation and identified areas for improvement
  - Created a five-phase implementation plan with specific tasks for each phase
  - Updated todo.md with detailed tasks for each phase of the enhancement
  - Updated context-summary.md with comprehensive development plan
  - Identified CPU configuration discrepancies between Kconfig files
  - Planned hardware abstraction layer for consistent API across environments
  - Designed approach for enhanced application functionality in QEMU
  - Test results: Plan ready for implementation with clear, actionable tasks

- Fixed QEMU emulation for STM32H573I-DK application

  - Resolved CPU configuration issues by switching to qemu_cortex_m3 board target
  - Modified run-qemu.ps1 script to use Cortex-M3 CPU with mps2-an385 machine
  - Removed direct CPU_CORTEX_M33 configuration that was causing build errors
  - Successfully built and ran the application in QEMU
  - Test results: Application now runs in QEMU without errors

- Corrected processor architecture for STM32H573I-DK from Cortex-M7 to Cortex-M33

  - Updated all configuration files to use the correct Cortex-M33 processor
  - Created new qemu_cortex_m33.conf configuration file
  - Updated Kconfig.board to select CPU_CORTEX_M33
  - Modified run-qemu.ps1 to use Cortex-M33 CPU in QEMU
  - Updated documentation to reflect the correct processor architecture
  - Test results: Configuration corrected to match the actual STM32H573I-DK hardware

- Configured Zephyr QEMU for STM32H573I-DK emulation

  - Created a basic Zephyr application for the STM32H573I-DK board
  - Implemented custom configuration for running STM32H573I-DK in QEMU
  - Created device tree overlay for the STM32H573I-DK
  - Developed PowerShell script for building and running the application in QEMU
  - Added comprehensive documentation on the emulation approach
  - Test results: Application successfully configured to use mps2-an500 machine for STM32H573I-DK emulation

- Improved QEMU setup for STM32H573I-DK emulation

  - Redesigned PowerShell script (tools/setup-qemu.ps1) to guide through manual installation
  - Added automatic creation of installation guide with step-by-step instructions
  - Created run-qemu.ps1 script for easy launching of emulated STM32H573I-DK
  - Implemented configuration and verification of QEMU installation
  - Added test program generation for verifying STM32 emulation capability
  - Test results: Script successfully detects existing installations, guides manual installation, and configures QEMU for STM32H573I-DK emulation

- Updated Rust crypto library to support ARM Cortex-M33 target (STM32H573I-DK)

  - Added feature flags for conditional compilation (std vs embedded)
  - Configured dependencies to work without the standard library
  - Added embedded-specific dependencies (cortex-m, cortex-m-rt, stm32h5)
  - Implemented no_std support with hardware acceleration hooks
  - Modified build.rs to detect target architecture and adjust output

- Created comprehensive QEMU integration plan for STM32H573I-DK emulation (Documentation/Features/QEMU_Integration.md)

  - Detailed four-phase implementation approach for emulating the STM32H573I-DK
  - Included development and testing prompts for each implementation step
  - Added implementation timeline and mermaid diagrams for visualization
  - Provided guidance for hardware crypto acceleration emulation

- Added CMake toolchain file for STM32H573I-DK target (cmake/arm-none-eabi.cmake)
  - Configured for ARM Cortex-M33 processor
  - Set up cross-compilation with arm-none-eabi-gcc
  - Added embedded-specific compiler and linker flags
  - Implemented add_stm32_executable helper function
- Updated DEVELOPER_SETUP.md with comprehensive embedded development setup instructions
  - Added detailed steps for installing CMake, ARM GNU Toolchain, MSYS2, MinGW, and Ninja
  - Included instructions for verifying the ARM toolchain configuration
  - Added guidance for building and flashing firmware to the STM32H573I-DK
- Tested ARM toolchain configuration
  - Created a simple test project with a minimal C++ file
  - Successfully compiled and linked for ARM Cortex-M33 target
  - Verified ELF file format is suitable for STM32H573I-DK
