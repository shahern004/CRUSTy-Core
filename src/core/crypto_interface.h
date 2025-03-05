// This is a placeholder file that will be replaced by the auto-generated header from cbindgen
// when the project is built. It contains the C interface to the Rust cryptographic functions.

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
