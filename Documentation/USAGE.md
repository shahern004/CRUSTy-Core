# CRUSTy-Core Usage Guide

This guide provides detailed instructions for using the CRUSTy-Core file encryption application.

## Table of Contents

1. [Installation](#installation)
2. [Basic Usage](#basic-usage)
   - [Encrypting Files](#encrypting-files)
   - [Decrypting Files](#decrypting-files)
3. [Advanced Features](#advanced-features)
   - [Two-Factor Authentication](#two-factor-authentication)
   - [Batch Processing](#batch-processing)
4. [Security Considerations](#security-considerations)
5. [Troubleshooting](#troubleshooting)

## Installation

### System Requirements

- Windows 10/11, macOS 10.15+, or Linux (Ubuntu 20.04+, Fedora 34+)
- 64-bit processor
- 4GB RAM minimum
- 100MB disk space

### Installation Steps

1. Download the latest release from the [releases page](https://github.com/yourusername/CRUSTy-Core/releases).
2. Run the installer for your platform:
   - Windows: Double-click the `.exe` file and follow the installation wizard.
   - macOS: Mount the `.dmg` file, drag the application to your Applications folder.
   - Linux: Extract the `.tar.gz` file and run the install script: `./install.sh`

## Basic Usage

### Encrypting Files

1. Launch CRUSTy-Core.
2. Click the "Select File" button in the Encrypt section.
3. Choose the file you want to encrypt.
4. Select an output directory or use the default (same directory as the input file).
5. Enter a strong password in the password field.
6. Click the "Encrypt" button.
7. Wait for the encryption process to complete.
8. Your encrypted file will be saved with a `.crusty` extension.

### Decrypting Files

1. Launch CRUSTy-Core.
2. Click the "Select File" button in the Decrypt section.
3. Choose the `.crusty` file you want to decrypt.
4. Select an output directory or use the default.
5. Enter the password used for encryption.
6. If two-factor authentication was enabled, enter the second factor.
7. Click the "Decrypt" button.
8. Wait for the decryption process to complete.
9. Your decrypted file will be saved to the specified output directory.

## Advanced Features

### Two-Factor Authentication

CRUSTy-Core supports two-factor authentication (2FA) for enhanced security:

1. To enable 2FA, check the "Enable 2FA" option when encrypting a file.
2. You'll be prompted to set up 2FA using a compatible authenticator app (like Google Authenticator or Authy).
3. Scan the QR code with your authenticator app.
4. When decrypting the file, you'll need both your password and the current code from your authenticator app.

### Batch Processing

To encrypt or decrypt multiple files at once:

1. Click the "Batch Mode" button.
2. Select multiple files for processing.
3. Choose an output directory.
4. Enter your password (and second factor if applicable).
5. Click "Process Files".
6. Monitor the progress in the batch processing window.

## Security Considerations

- **Password Strength**: Use a strong, unique password for each important file.
- **Password Storage**: CRUSTy-Core does not store your passwords. If you forget your password, your encrypted data cannot be recovered.
- **Second Factor**: If you enable 2FA, ensure you have a backup of your authenticator app or recovery codes.
- **Secure Environment**: Ensure your computer is free from malware when encrypting/decrypting sensitive files.
- **Disclaimer**: While CRUSTy-Core uses strong encryption (AES-256-GCM), it has not been formally audited for security vulnerabilities. Use at your own risk for highly sensitive data.

## Troubleshooting

### Common Issues

1. **"Authentication Failed" Error**: This usually means the password is incorrect. Double-check your password and try again.

2. **"File Access Error"**: Ensure you have the necessary permissions to read the input file and write to the output directory.

3. **"Invalid File Format" Error**: The file you're trying to decrypt may be corrupted or not encrypted with CRUSTy-Core.

4. **Performance Issues**: When encrypting very large files, ensure your system has sufficient memory and disk space.

### Getting Help

If you encounter issues not covered in this guide:

1. Check the [GitHub Issues](https://github.com/yourusername/CRUSTy-Core/issues) for similar problems and solutions.
2. Submit a new issue with detailed information about your problem, including:
   - Your operating system and CRUSTy-Core version
   - Steps to reproduce the issue
   - Any error messages you received
   - Log files (if available)
