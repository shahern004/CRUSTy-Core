# CRUSTy-Core Changelog

This file documents all significant changes to the CRUSTy-Core project.

## 2025-03-10

- Performed repository directory cleanup
  - Organized C and H files while maintaining the existing pathing in scripts/configurations
  - Created reference document (organize_files.md) identifying primary files for STM32 and QEMU builds
  - Updated .gitignore to exclude unused Zephyr modules and build directories
  - Created and executed cleanup_unused_files.ps1 to remove redundant directories
  - Identified and preserved essential build and test directories
  - Reduced repository size by removing unnecessary build artifacts

## 2025-03-09

- Fixed QEMU build and CLI testing issues

  - Created diagnostic script to identify Zephyr workspace location issues
  - Fixed path handling in build scripts to properly use Zephyr workspace directory
  - Created simplified minimal CLI test application for reliable QEMU testing
  - Resolved CMake configuration file path issues by using forward slashes consistently
  - Implemented proper directory structure navigation for cross-platform compatibility
  - Successfully tested CLI commands in QEMU environment
  - Consolidated multiple test scripts into a clean, reliable test framework

- Added continuous codebase cleanup approach to development process

  - Updated STM32H573I-DK_Migration_Roadmap.md with codebase organization guidelines
  - Established process for iterative cleanup after each functional component
  - Created recommendations for script consolidation and documentation
  - Implemented best practices for maintaining clean directory structure
  - Added guidance for consistent naming conventions and code organization

- Implemented Zephyr Shell Command Interface for cryptographic operations

  - Created shell_cmds.c and shell_cmds.h with complete command handler structure
  - Added commands for AES-GCM encryption/decryption, SHA-256 hashing, and random number generation
  - Added crypto status command to show hardware acceleration capabilities
  - Added crypto self-test command to verify crypto subsystem operation
  - Added hex string conversion utilities for command input/output
  - Updated prj.conf to enable shell subsystem and configure shell parameters
  - Updated main.c to initialize the shell commands module

- Implemented Dual Platform HAL approach for STM32H573I-DK and QEMU simulation
  - Refactored crypto_ops.c to use hardware acceleration on STM32H573I-DK with software fallbacks
  - Refactored gpio_control.c to support both real hardware and QEMU simulation
  - Refactored uart_comm.c to provide hardware and simulated communication
  - Refactored mem_protect.c to provide hardware MPU support on STM32H573I-DK and simulation in QEMU
  - Updated CMakeLists.txt to use conditional compilation flags based on target platform
  - Updated prj.conf with platform-aware configuration for both STM32H573I-DK and QEMU
  - Added build_stm32.ps1 script for dedicated hardware builds alongside run_qemu.ps1
- Fixed CMake path inconsistency issue by cleaning and reconfiguring the build
- Updated STM32H573I-DK_Migration_Roadmap.md to mark Hardware Security Feature Verification task as completed
- Fixed Rust dependency issue in Cargo.toml by removing non-existent 'std' feature from thiserror crate
- Fixed Rust binding generation by bypassing cbindgen and implementing a manual header file generation for crypto_interface.h
- Fixed Rust AES-GCM dependency issues by adding required dependencies (aes, generic-array) and updating import statements
- Fixed C++ namespace issues by properly qualifying Rust FFI function calls with the crusty::crypto namespace
- Implemented Zephyr Crypto Subsystem Integration for STM32H573I-DK with the following features:
  - Hardware-accelerated AES-GCM encryption/decryption with software fallback
  - Hardware-accelerated random number generation with software fallback
  - Software SHA-256 implementation (hardware not available on this platform)
  - Comprehensive crypto operations demo with performance benchmarking
