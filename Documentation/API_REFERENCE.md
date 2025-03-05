# CRUSTy-Core API Reference

This document provides a comprehensive reference for the CRUSTy-Core API, covering both the C++ and Rust components and their interfaces.

## Table of Contents

- [C++ API](#c-api)
  - [Encryptor Class](#encryptor-class)
  - [FileOperations Class](#fileoperations-class)
  - [MainWindow Class](#mainwindow-class)
- [Rust API](#rust-api)
  - [Cryptographic Functions](#cryptographic-functions)
  - [Error Handling](#error-handling)
- [FFI Boundary](#ffi-boundary)
  - [C Interface](#c-interface)
  - [Type Mapping](#type-mapping)
  - [Memory Management](#memory-management)

## C++ API

### Encryptor Class

The `Encryptor` class provides a C++ wrapper around the Rust cryptographic functions, offering a clean, object-oriented interface for encryption operations.

#### Constructor

```cpp
Encryptor()
```

Creates a new `Encryptor` instance.

#### File Encryption

```cpp
void encryptFile(
    const std::string& sourcePath,
    const std::string& destPath,
    const std::string& password,
    std::function<void(float)> progressCallback = nullptr
)
```

Encrypts a file with a password.

- **Parameters**:
  - `sourcePath`: Path to the source file
  - `destPath`: Path to the destination file
  - `password`: Password for encryption
  - `progressCallback`: Optional callback function for progress updates (0.0 to 1.0)
- **Throws**: `EncryptionException` if encryption fails

#### File Decryption

```cpp
void decryptFile(
    const std::string& sourcePath,
    const std::string& destPath,
    const std::string& password,
    const std::string& secondFactor = "",
    std::function<void(float)> progressCallback = nullptr
)
```

Decrypts a file with a password.

- **Parameters**:
  - `sourcePath`: Path to the source file
  - `destPath`: Path to the destination file
  - `password`: Password for decryption
  - `secondFactor`: Optional second factor for authentication
  - `progressCallback`: Optional callback function for progress updates (0.0 to 1.0)
- **Throws**: `EncryptionException` if decryption fails

#### Data Encryption

```cpp
std::vector<uint8_t> encryptData(
    const std::vector<uint8_t>& data,
    const std::string& password
)
```

Encrypts raw data with a password.

- **Parameters**:
  - `data`: Data to encrypt
  - `password`: Password for encryption
- **Returns**: Encrypted data
- **Throws**: `EncryptionException` if encryption fails

#### Data Decryption

```cpp
std::vector<uint8_t> decryptData(
    const std::vector<uint8_t>& data,
    const std::string& password
)
```

Decrypts raw data with a password.

- **Parameters**:
  - `data`: Data to decrypt
  - `password`: Password for decryption
- **Returns**: Decrypted data
- **Throws**: `EncryptionException` if decryption fails

#### Password Hashing

```cpp
std::string hashPassword(const std::string& password)
```

Hashes a password for verification.

- **Parameters**:
  - `password`: Password to hash
- **Returns**: Hashed password
- **Throws**: `EncryptionException` if hashing fails

#### Password Verification

```cpp
bool verifyPassword(const std::string& password, const std::string& hash)
```

Verifies a password against a hash.

- **Parameters**:
  - `password`: Password to verify
  - `hash`: Hash to verify against
- **Returns**: `true` if password matches hash, `false` otherwise

### FileOperations Class

The `FileOperations` class provides functionality for file operations like selecting files, creating output directories, and managing file paths.

#### File Selection

```cpp
static std::string selectFile(
    const std::string& title,
    const std::string& filter,
    bool isOpen = true
)
```

Selects a file for encryption/decryption.

- **Parameters**:
  - `title`: Dialog title
  - `filter`: File filter (e.g., "All Files (*.*)")
  - `isOpen`: `true` for open dialog, `false` for save dialog
- **Returns**: Selected file path, or empty string if canceled

#### Multiple File Selection

```cpp
static std::vector<std::string> selectFiles(
    const std::string& title,
    const std::string& filter
)
```

Selects multiple files for encryption/decryption.

- **Parameters**:
  - `title`: Dialog title
  - `filter`: File filter (e.g., "All Files (*.*)")
- **Returns**: Vector of selected file paths, or empty vector if canceled

#### Directory Selection

```cpp
static std::string selectDirectory(const std::string& title)
```

Selects a directory.

- **Parameters**:
  - `title`: Dialog title
- **Returns**: Selected directory path, or empty string if canceled

#### Default Output Path

```cpp
static std::string getDefaultOutputPath(
    const std::string& sourcePath,
    bool isEncrypting
)
```

Gets the default output path for an encrypted/decrypted file.

- **Parameters**:
  - `sourcePath`: Source file path
  - `isEncrypting`: `true` if encrypting, `false` if decrypting
- **Returns**: Default output path

#### Directory Creation

```cpp
static void createDirectory(const std::string& path)
```

Creates a directory if it doesn't exist.

- **Parameters**:
  - `path`: Directory path
- **Throws**: `FileOperationException` if directory creation fails

#### File Existence Check

```cpp
static bool fileExists(const std::string& path)
```

Checks if a file exists.

- **Parameters**:
  - `path`: File path
- **Returns**: `true` if file exists, `false` otherwise

#### File Size

```cpp
static std::uintmax_t getFileSize(const std::string& path)
```

Gets the file size.

- **Parameters**:
  - `path`: File path
- **Returns**: File size in bytes
- **Throws**: `FileOperationException` if file doesn't exist or can't be accessed

#### File Name Extraction

```cpp
static std::string getFileName(const std::string& path)
```

Gets the file name from a path.

- **Parameters**:
  - `path`: File path
- **Returns**: File name

#### File Extension Extraction

```cpp
static std::string getFileExtension(const std::string& path)
```

Gets the file extension from a path.

- **Parameters**:
  - `path`: File path
- **Returns**: File extension

#### Directory Path Extraction

```cpp
static std::string getDirectoryPath(const std::string& path)
```

Gets the directory path from a file path.

- **Parameters**:
  - `path`: File path
- **Returns**: Directory path

### MainWindow Class

The `MainWindow` class implements the main application window using Qt.

#### Constructor

```cpp
explicit MainWindow(QWidget *parent = nullptr)
```

Creates a new `MainWindow` instance.

- **Parameters**:
  - `parent`: Optional parent widget

#### Public Slots

```cpp
void selectEncryptFile()
```

Selects a file for encryption.

```cpp
void selectDecryptFile()
```

Selects a file for decryption.

```cpp
void selectEncryptOutput()
```

Selects output directory for encryption.

```cpp
void selectDecryptOutput()
```

Selects output directory for decryption.

```cpp
void encryptFile()
```

Encrypts the selected file.

```cpp
void decryptFile()
```

Decrypts the selected file.

```cpp
void showAbout()
```

Shows the about dialog.

## Rust API

### Cryptographic Functions

#### Encrypt Data

```rust
#[no_mangle]
pub unsafe extern "C" fn encrypt_data(
    data_ptr: *const u8, data_len: usize,
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_max_len: usize,
    output_len: *mut usize
) -> i32
```

Encrypts data using AES-256-GCM with the provided password.

- **Parameters**:
  - `data_ptr`: Pointer to the data to encrypt
  - `data_len`: Length of the data
  - `password_ptr`: Pointer to the password
  - `password_len`: Length of the password
  - `output_ptr`: Pointer to the output buffer
  - `output_max_len`: Maximum length of the output buffer
  - `output_len`: Pointer to store the actual output length
- **Returns**: Error code (0 for success, negative for errors)

#### Decrypt Data

```rust
#[no_mangle]
pub unsafe extern "C" fn decrypt_data(
    data_ptr: *const u8, data_len: usize,
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_max_len: usize,
    output_len: *mut usize
) -> i32
```

Decrypts data using AES-256-GCM with the provided password.

- **Parameters**:
  - `data_ptr`: Pointer to the data to decrypt
  - `data_len`: Length of the data
  - `password_ptr`: Pointer to the password
  - `password_len`: Length of the password
  - `output_ptr`: Pointer to the output buffer
  - `output_max_len`: Maximum length of the output buffer
  - `output_len`: Pointer to store the actual output length
- **Returns**: Error code (0 for success, negative for errors)

#### Hash Password

```rust
#[no_mangle]
pub unsafe extern "C" fn hash_password(
    password_ptr: *const u8, password_len: usize,
    output_ptr: *mut u8, output_len: usize
) -> i32
```

Hashes a password using Argon2id for verification.

- **Parameters**:
  - `password_ptr`: Pointer to the password
  - `password_len`: Length of the password
  - `output_ptr`: Pointer to the output buffer
  - `output_len`: Length of the output buffer
- **Returns**: Error code (0 for success, negative for errors)

#### Derive Key from Password

```rust
#[no_mangle]
pub unsafe extern "C" fn derive_key_from_password(
    password_ptr: *const u8, password_len: usize,
    salt_ptr: *const u8, salt_len: usize,
    key_ptr: *mut u8, key_len: usize
) -> i32
```

Derives an encryption key from a password and salt.

- **Parameters**:
  - `password_ptr`: Pointer to the password
  - `password_len`: Length of the password
  - `salt_ptr`: Pointer to the salt
  - `salt_len`: Length of the salt
  - `key_ptr`: Pointer to the output key buffer
  - `key_len`: Length of the key buffer
- **Returns**: Error code (0 for success, negative for errors)

### Error Handling

```rust
#[repr(C)]
pub enum CryptoErrorCode {
    Success = 0,
    InvalidParams = -1,
    AuthenticationFailed = -2,
    EncryptionError = -3,
    DecryptionError = -4,
    KeyDerivationError = -5,
    BufferTooSmall = -6,
    InternalError = -7,
}
```

Error codes for cryptographic operations.

## FFI Boundary

### C Interface

The C interface to the Rust cryptographic functions is defined in `src/cpp/core/crypto_interface.h` and generated by cbindgen from the Rust code.

```c
// Error codes for cryptographic operations
enum CryptoErrorCode {
    // Operation completed successfully
    Success = 0,
    // Invalid parameters provided
    InvalidParams = -1,
    // Authentication failed during decryption
    AuthenticationFailed = -2,
    // Error during encryption
    EncryptionError = -3,
    // Error during decryption
    DecryptionError = -4,
    // Error with key derivation
    KeyDerivationError = -5,
    // Buffer too small for output
    BufferTooSmall = -6,
    // Internal error
    InternalError = -7,
};

// Encrypts data using AES-256-GCM with the provided password
int32_t encrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

// Decrypts data using AES-256-GCM with the provided password
int32_t decrypt_data(
    const uint8_t* data_ptr, size_t data_len,
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_max_len,
    size_t* output_len
);

// Hashes a password using Argon2id for verification
int32_t hash_password(
    const uint8_t* password_ptr, size_t password_len,
    uint8_t* output_ptr, size_t output_len
);

// Derives an encryption key from a password and salt
int32_t derive_key_from_password(
    const uint8_t* password_ptr, size_t password_len,
    const uint8_t* salt_ptr, size_t salt_len,
    uint8_t* key_ptr, size_t key_len
);
```

### Type Mapping

| Rust Type | C Type | C++ Type |
|-----------|--------|----------|
| `u8` | `uint8_t` | `uint8_t` |
| `i32` | `int32_t` | `int32_t` |
| `usize` | `size_t` | `size_t` |
| `*const u8` | `const uint8_t*` | `const uint8_t*` |
| `*mut u8` | `uint8_t*` | `uint8_t*` |
| `*mut usize` | `size_t*` | `size_t*` |
| `enum CryptoErrorCode` | `enum CryptoErrorCode` | `enum CryptoErrorCode` |

### Memory Management

- **Ownership**: The C++ code is responsible for allocating and freeing memory for input and output buffers.
- **Buffer Sizing**: If an output buffer is too small, the function returns `BufferTooSmall` and sets `output_len` to the required size.
- **String Handling**: Strings are passed as pointers with explicit lengths, not null-terminated.
- **Lifetime**: All pointers must remain valid for the duration of the function call.
