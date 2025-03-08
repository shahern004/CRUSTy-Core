# CRUSTy-Core

Secure file encryption application using AES-256-GCM with a hybrid C++/Rust architecture.

## Core Components

- C++ application logic
- Rust cryptographic operations
- Hardware acceleration support for STM32H573I-DK

## Documentation

- [Developer Setup](Documentation/Guides/DEVELOPER_SETUP.md) - Environment setup
- [API Reference](Documentation/Guides/API_REFERENCE.md) - API documentation
- [QEMU Emulation](Documentation/QEMU_Unified_Guide.md) - STM32H573I-DK emulation
- [Embedded Development](Documentation/Embedded_Unified_Guide.md) - STM32H573I-DK integration

## Features

- AES-256-GCM authenticated encryption
- Single file and batch processing
- Key generation, storage, and loading
- Progress tracking and operation logging
- Cross-platform (Windows, macOS, Linux)
- STM32H573I-DK hardware acceleration

## Security

- Rust for memory-safe cryptographic operations
- Secure memory handling with automatic wiping
- [Detailed security documentation](Documentation/Security/SECURITY_IMPLEMENTATION.md)

## License

MIT License - see LICENSE file for details.

## Disclaimer

Educational proof-of-concept. Not audited for security vulnerabilities.
