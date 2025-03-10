/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CRYPTO_OPS_H_
#define CRYPTO_OPS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Initialize the crypto subsystem
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_init(void);

/**
 * @brief Get information about available crypto hardware
 * 
 * @param[out] has_hw_aes Set to true if hardware AES is available
 * @param[out] has_hw_rng Set to true if hardware RNG is available
 * @param[out] has_hw_sha Set to true if hardware SHA is available
 * @param[out] has_hw_pka Set to true if hardware PKA is available
 */
void crypto_ops_get_capabilities(bool *has_hw_aes, bool *has_hw_rng, 
                               bool *has_hw_sha, bool *has_hw_pka);

/**
 * @brief AES-GCM encryption function
 * 
 * @param[in] key Pointer to the key
 * @param[in] key_len Length of the key in bytes (16, 24, or 32)
 * @param[in] nonce Pointer to the nonce (IV)
 * @param[in] nonce_len Length of the nonce in bytes (typically 12)
 * @param[in] aad Pointer to the additional authenticated data
 * @param[in] aad_len Length of the additional authenticated data in bytes
 * @param[in] plaintext Pointer to the plaintext data
 * @param[in] plaintext_len Length of the plaintext in bytes
 * @param[out] ciphertext Pointer to the buffer for the ciphertext
 * @param[in,out] ciphertext_len Length of the ciphertext buffer / Length of the ciphertext
 * @param[out] tag Pointer to the buffer for the authentication tag
 * @param[in] tag_len Length of the tag in bytes (typically 16)
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_aes_gcm_encrypt(const uint8_t *key, size_t key_len,
                             const uint8_t *nonce, size_t nonce_len,
                             const uint8_t *aad, size_t aad_len,
                             const uint8_t *plaintext, size_t plaintext_len,
                             uint8_t *ciphertext, size_t *ciphertext_len,
                             uint8_t *tag, size_t tag_len);

/**
 * @brief AES-GCM decryption function
 * 
 * @param[in] key Pointer to the key
 * @param[in] key_len Length of the key in bytes (16, 24, or 32)
 * @param[in] nonce Pointer to the nonce (IV)
 * @param[in] nonce_len Length of the nonce in bytes (typically 12)
 * @param[in] aad Pointer to the additional authenticated data
 * @param[in] aad_len Length of the additional authenticated data in bytes
 * @param[in] ciphertext Pointer to the ciphertext data
 * @param[in] ciphertext_len Length of the ciphertext in bytes
 * @param[in] tag Pointer to the authentication tag
 * @param[in] tag_len Length of the tag in bytes
 * @param[out] plaintext Pointer to the buffer for the plaintext
 * @param[in,out] plaintext_len Length of the plaintext buffer / Length of the plaintext
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_aes_gcm_decrypt(const uint8_t *key, size_t key_len,
                             const uint8_t *nonce, size_t nonce_len,
                             const uint8_t *aad, size_t aad_len,
                             const uint8_t *ciphertext, size_t ciphertext_len,
                             const uint8_t *tag, size_t tag_len,
                             uint8_t *plaintext, size_t *plaintext_len);

/**
 * @brief SHA-256 hash function
 * 
 * @param[in] data Pointer to the data to hash
 * @param[in] data_len Length of the data in bytes
 * @param[out] hash Pointer to the buffer for the hash (must be at least 32 bytes)
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_sha256(const uint8_t *data, size_t data_len, uint8_t *hash);

/**
 * @brief Generate random bytes
 * 
 * @param[out] buffer Pointer to the buffer for the random bytes
 * @param[in] len Length of the buffer in bytes
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_random_bytes(uint8_t *buffer, size_t len);

/**
 * @brief Run a self-test of the crypto operations
 * 
 * @return 0 on success, negative value on error
 */
int crypto_ops_self_test(void);

#endif /* CRYPTO_OPS_H_ */
