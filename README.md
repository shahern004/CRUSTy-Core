# CRUSTy - A Secure File Encryption Application

CRUSTy is a robust, secure file encryption application that implements AES-256-GCM authenticated encryption with a focus on security, usability, and extensibility. This repository (CRUSTy-Core) contains the hybrid C++/Rust implementation, which combines:

- A modern Qt-based user interface (C++)
- Core application logic in C++
- Security-critical cryptographic operations in Rust
- Two-factor authentication for enhanced security

It provides a comprehensive solution for protecting sensitive data through strong cryptographic primitives while maintaining a user-friendly interface.

## Quick Start

For detailed installation and usage instructions, please see the [Usage Guide](Documentation/USAGE.md).

## Features

- **Strong Encryption**: Uses AES-256-GCM for secure, authenticated encryption
- **Modern User Interface**: Sleek and intuitive GUI built with Qt 6
- **Two-Factor Authentication**: Enhanced security with TOTP-based second factor
- **Flexible Operation Modes**:
  - Single file encryption/decryption
  - Batch processing for multiple files
- **Key Management**:
  - Generate new encryption keys
  - Save keys to files for later use
  - Load keys from files
- **Progress Tracking**: Real-time progress indicators for encryption/decryption operations
- **Operation Logging**: Detailed logs of all encryption and decryption operations
- **Error Handling**: Clear error messages and prevention of corrupted output files
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Security Considerations

- DO NOT use CRUSTy for critical file encryption. This is purely an educational proof-of-concept
- CRUSTy uses AES-256-GCM, a secure authenticated encryption algorithm
- Each file is encrypted with a unique nonce to prevent replay attacks
- The application has not been formally audited for security vulnerabilities
- For highly sensitive data, consider using established encryption tools

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Disclaimer

I built CRUSTy for my own learning, but if it can help others that is an amazing benefit. While it uses strong encryption algorithms, it has not been audited for security vulnerabilities. Use at your own risk for sensitive data.
