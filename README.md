# CRUSTy-Core - A Secure File Encryption Application

CRUSTy-Core is a file encryption application that implements AES-256-GCM authenticated encryption with a focus on security, usability, and extensibility. This repository contains the hybrid C++/Rust implementation, which combines:

- Core application logic in C++
- Security-critical cryptographic operations in Rust

## Quick Start

For detailed installation and usage instructions, please see the [Usage Guide](Documentation/USAGE.md).

## Documentation

- [Usage Guide](Documentation/USAGE.md) - Instructions for end users
- [Developer Setup Guide](Documentation/DEVELOPER_SETUP.md) - Guide for setting up the development environment
- [API Reference](Documentation/API_REFERENCE.md) - Detailed API documentation

## Features

- **Strong Encryption**: Uses AES-256-GCM for secure, authenticated encryption
- **Flexible Operation Modes**:
  - Single file encryption/decryption
  - Batch processing for multiple files
- **Key Management**:
  - Generate new encryption keys
  - Save keys to files for later use
  - Load keys from files
- **Progress Tracking**: Real-time progress indicators for encryption/decryption operations
- **Operation Logging**: Detailed logs of all encryption and decryption operations
- **Error Handling**: Error messages and prevention of corrupted output files
- **Cross-Platform**: Works on Windows, macOS, and Linux

## Security Considerations

- DO NOT use CRUSTy-Core for critical file encryption. This is purely an educational proof-of-concept
- CRUSTy-Core uses AES-256-GCM, a secure authenticated encryption algorithm
- Each file is encrypted with a unique nonce to prevent replay attacks
- The application has not been formally audited for security vulnerabilities
- For highly sensitive data, consider using established encryption tools

## NSA Secure Coding Practices

CRUSTy-Core implements secure coding practices aligned with NSA Guidelines:

- **Memory Safety Implementation**
  - Use of Rust for security-critical cryptographic operations
  - SecureData template with automatic memory wiping in C++
  - Smart pointers and RAII patterns for resource management

- **Input Validation**
  - Path sanitization to prevent directory traversal attacks
  - Comprehensive validation at trust boundaries

- **Error Handling**
  - Comprehensive exception handling with secure failure states
  - Secure default states on failure

- **Compiler Hardening**
  - Stack protection mechanisms to prevent buffer overflow attacks
  - Address Space Layout Randomization (ASLR) for increased memory access security
  - Data Execution Prevention (DEP) to prevent code execution from data pages
  - Control Flow Guard to prevent hijacking of program control flow
  - Position Independent Executable (PIE) support for enhanced security

- **Secure Logging**
  - Thread-safe audit logging system for security events

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Disclaimer

I built CRUSTy-Core for my own learning, but if it can help others that is an amazing benefit. While it uses strong encryption algorithms, it has not been audited for security vulnerabilities. Use at your own risk for sensitive data.
