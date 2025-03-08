# CRUSTy-Core Technology Stack

## Core Technologies

- **C++ (C++17)** - Main application language for core processing and system integration
- **Rust (1.56+)** - Memory-safe cryptographic operations and input validation
- **CMake (3.15+)** - Build system and cross-platform configuration

## Build & Integration Tools

- **Corrosion** - CMake integration for Rust components
- **cbindgen** - Automatic C/C++ header generation from Rust code

## Cryptographic Components

- **AES-256-GCM** - Via Rust's `aes-gcm` crate
- **Argon2id** - Password hashing and key derivation
- **Secure memory handling** - Explicit wiping of sensitive data

## Embedded Target Support

- **STM32H573I-DK** - ARM Cortex-M7 microcontroller
- **ARM GCC Toolchain** - Cross-compilation for embedded targets
- **STM32CubeH5 HAL** - Hardware abstraction layer for STM32H5 series

## Hardware Acceleration

- **AES-GCM Hardware Engine** - For encryption/decryption operations
- **SHA Hardware Accelerator** - For hashing operations
- **True Random Number Generator** - For cryptographic key generation
- **PKA (Public Key Accelerator)** - For asymmetric cryptography

## Development Tools

- **Git** - Version control
- **Visual Studio Code/CLion** - Recommended IDEs
- **Rust-analyzer** - Rust language support
- **C/C++ extensions** - C++ language support
