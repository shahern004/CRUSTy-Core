# CRUSTy-Core

Secure file encryption application using AES-256-GCM with a hybrid C++/Rust architecture.

## Core Components

- C++ application logic
- Rust cryptographic operations
- Hardware acceleration support for STM32H573I-DK

## Documentation

- [Developer Setup](Documentation/DEVELOPER_SETUP.md) - Environment setup
- [System Design](Documentation/Core/SYSTEM_DESIGN.md) - Architecture and design
- [API Reference](Documentation/Core/API_REFERENCE.md) - API documentation
- [Embedded Guide](Documentation/Embedded/EMBEDDED_GUIDE.md) - STM32H573I-DK integration
- [QEMU Guide](Documentation/Embedded/QEMU_GUIDE.md) - STM32H573I-DK emulation
- [STM32H573I Reference](Documentation/Embedded/STM32H573I_REFERENCE.md) - Hardware reference

See the [Documentation README](Documentation/README.md) for a complete list of documentation.

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
- [Detailed security documentation](Documentation/Core/SYSTEM_DESIGN.md#8-security-considerations)

## License

MIT License - see LICENSE file for details.

## Disclaimer

Educational proof-of-concept. Not audited for security vulnerabilities.
