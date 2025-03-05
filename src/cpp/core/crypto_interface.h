#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

namespace crusty {
namespace crypto {

/**
 * Error codes for cryptographic operations
 */
enum class CryptoErrorCode {
    /// Operation completed successfully
    Success = 0,
    /// Invalid parameters provided
    InvalidParams = -1,
    /// Authentication failed during decryption
    AuthenticationFailed = -2,
    /// Error during encryption
    EncryptionError = -3,
    /// Error during decryption
    DecryptionError = -4,
    /// Error with key derivation
    KeyDerivationError = -5,
    /// Buffer too small for output
    BufferTooSmall = -6,
    /// Internal error
    InternalError = -7,
};

} // namespace crypto
} // namespace crusty

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Encrypts data using AES-256-GCM with the provided password
 *
 * # Safety
 *
 * This function is unsafe because it dereferences raw pointers.
 * The caller must ensure that:
 * - `data_ptr` points to a valid buffer of at least `data_len` bytes
 * - `password_ptr` points to a valid buffer of at least `password_len` bytes
 * - `output_ptr` points to a buffer of at least `output_max_len` bytes
 * - `output_len` points to a valid `usize`
 */
int32_t encrypt_data(const uint8_t *data_ptr,
                     uintptr_t data_len,
                     const uint8_t *password_ptr,
                     uintptr_t password_len,
                     uint8_t *output_ptr,
                     uintptr_t output_max_len,
                     uintptr_t *output_len);

/**
 * Decrypts data using AES-256-GCM with the provided password
 *
 * # Safety
 *
 * This function is unsafe because it dereferences raw pointers.
 * The caller must ensure that:
 * - `data_ptr` points to a valid buffer of at least `data_len` bytes
 * - `password_ptr` points to a valid buffer of at least `password_len` bytes
 * - `output_ptr` points to a buffer of at least `output_max_len` bytes
 * - `output_len` points to a valid `usize`
 */
int32_t decrypt_data(const uint8_t *data_ptr,
                     uintptr_t data_len,
                     const uint8_t *password_ptr,
                     uintptr_t password_len,
                     uint8_t *output_ptr,
                     uintptr_t output_max_len,
                     uintptr_t *output_len);

/**
 * Hashes a password using Argon2id for verification
 *
 * # Safety
 *
 * This function is unsafe because it dereferences raw pointers.
 * The caller must ensure that:
 * - `password_ptr` points to a valid buffer of at least `password_len` bytes
 * - `output_ptr` points to a buffer of at least `output_len` bytes
 */
int32_t hash_password(const uint8_t *password_ptr,
                      uintptr_t password_len,
                      uint8_t *output_ptr,
                      uintptr_t output_len);

/**
 * Derives an encryption key from a password and salt
 *
 * # Safety
 *
 * This function is unsafe because it dereferences raw pointers.
 * The caller must ensure that:
 * - `password_ptr` points to a valid buffer of at least `password_len` bytes
 * - `salt_ptr` points to a valid buffer of at least `salt_len` bytes
 * - `key_ptr` points to a buffer of at least `key_len` bytes
 */
int32_t derive_key_from_password(const uint8_t *password_ptr,
                                 uintptr_t password_len,
                                 const uint8_t *salt_ptr,
                                 uintptr_t salt_len,
                                 uint8_t *key_ptr,
                                 uintptr_t key_len);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
