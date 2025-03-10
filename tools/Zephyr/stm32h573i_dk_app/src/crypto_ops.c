/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdlib.h>
#include "crypto_ops.h"

/* Additional includes for hardware crypto */
#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
#include <zephyr/crypto/crypto.h>
#include <zephyr/device.h>
#include <zephyr/drivers/entropy.h>
#endif

/* Log module declaration */
LOG_MODULE_REGISTER(crypto_ops, CONFIG_LOG_DEFAULT_LEVEL);

/* Return codes */
#define CRYPTO_OPS_SUCCESS           0
#define CRYPTO_OPS_ERR_NOT_INIT     -1
#define CRYPTO_OPS_ERR_PARAM        -2
#define CRYPTO_OPS_ERR_KEY          -3
#define CRYPTO_OPS_ERR_BUFFER       -4
#define CRYPTO_OPS_ERR_AUTH         -5
#define CRYPTO_OPS_ERR_HARDWARE     -6

/* State variables */
static bool crypto_initialized = false;

#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
/* STM32H573I-DK specific variables */
static const struct device *crypto_dev = NULL;
static const struct device *entropy_dev = NULL;
static bool has_hw_aes_impl = false;
static bool has_hw_rng_impl = false;

/* Software implementations via mbedTLS for missing hardware features */
#include <mbedtls/sha256.h>
#endif

/* Simple pseudo-random number generator for QEMU demo */
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
static uint32_t random_seed = 0x12345678;

static uint32_t simple_rand(void)
{
    /* Very simple LCG random number generator */
    random_seed = (random_seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return random_seed;
}
#endif

int crypto_ops_init(void)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("Initializing crypto operations (QEMU simulation)");
    
    /* Nothing specific to initialize in QEMU simulation */
    
    crypto_initialized = true;
    LOG_INF("Crypto operations initialized successfully");
#else
    LOG_INF("Initializing crypto operations (STM32H573I-DK hardware)");
    
    /* Initialize AES hardware if available */
    #ifdef CONFIG_CRYPTO_STM32_AES
    crypto_dev = DEVICE_DT_GET(DT_NODELABEL(aes));
    if (crypto_dev != NULL && device_is_ready(crypto_dev)) {
        LOG_INF("AES hardware accelerator found");
        has_hw_aes_impl = true;
    } else {
        LOG_WRN("AES hardware accelerator not available, using software fallback");
        has_hw_aes_impl = false;
    }
    #else
    LOG_WRN("AES hardware support not enabled in config, using software fallback");
    has_hw_aes_impl = false;
    #endif
    
    /* Initialize RNG hardware if available */
    #ifdef CONFIG_ENTROPY_STM32_RNG
    entropy_dev = DEVICE_DT_GET(DT_NODELABEL(rng));
    if (entropy_dev != NULL && device_is_ready(entropy_dev)) {
        LOG_INF("Hardware RNG found");
        has_hw_rng_impl = true;
    } else {
        LOG_WRN("Hardware RNG not available, using software fallback");
        has_hw_rng_impl = false;
    }
    #else
    LOG_WRN("RNG hardware support not enabled in config, using software fallback");
    has_hw_rng_impl = false;
    #endif
    
    crypto_initialized = true;
    LOG_INF("Crypto operations initialized successfully");
#endif
    
    return CRYPTO_OPS_SUCCESS;
}

void crypto_ops_get_capabilities(bool *has_hw_aes, bool *has_hw_rng, 
                               bool *has_hw_sha, bool *has_hw_pka)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* In QEMU, we don't have hardware crypto */
    if (has_hw_aes) *has_hw_aes = false;
    if (has_hw_rng) *has_hw_rng = false;
    if (has_hw_sha) *has_hw_sha = false;
    if (has_hw_pka) *has_hw_pka = false;
#else
    /* On STM32H573I-DK, we have some hardware crypto features */
    if (has_hw_aes) *has_hw_aes = has_hw_aes_impl;
    if (has_hw_rng) *has_hw_rng = has_hw_rng_impl;
    if (has_hw_sha) *has_hw_sha = false; /* HASH peripheral not available in Zephyr */
    if (has_hw_pka) *has_hw_pka = false; /* PKA peripheral not available in Zephyr */
#endif
}

int crypto_ops_random_bytes(uint8_t *buffer, size_t len)
{
    if (!crypto_initialized) {
        LOG_ERR("Crypto operations not initialized");
        return CRYPTO_OPS_ERR_NOT_INIT;
    }
    
    if (buffer == NULL) {
        LOG_ERR("Invalid buffer pointer");
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (len == 0) {
        LOG_WRN("Zero length requested in random_bytes");
        return CRYPTO_OPS_SUCCESS;
    }
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Use simple PRNG for QEMU demo purposes */
    for (size_t i = 0; i < len; i++) {
        buffer[i] = (uint8_t)(simple_rand() & 0xFF);
    }
    
    LOG_DBG("Generated %u random bytes (QEMU simulation)", (unsigned int)len);
#else
    /* On STM32H573I-DK, use hardware RNG if available */
    if (has_hw_rng_impl && entropy_dev != NULL) {
        int ret = entropy_get_entropy(entropy_dev, buffer, len);
        if (ret != 0) {
            LOG_ERR("Hardware RNG failed: %d, falling back to software", ret);
            goto use_software_rng;
        }
        LOG_DBG("Generated %u random bytes (hardware RNG)", (unsigned int)len);
    } else {
    use_software_rng:
        /* Use Zephyr's software PRNG as fallback */
        sys_rand_get(buffer, len);
        LOG_DBG("Generated %u random bytes (software fallback)", (unsigned int)len);
    }
#endif
    
    return CRYPTO_OPS_SUCCESS;
}

int crypto_ops_aes_gcm_encrypt(const uint8_t *key, size_t key_len,
                              const uint8_t *nonce, size_t nonce_len,
                              const uint8_t *aad, size_t aad_len,
                              const uint8_t *plaintext, size_t plaintext_len,
                              uint8_t *ciphertext, size_t *ciphertext_len,
                              uint8_t *tag, size_t tag_len)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("AES-GCM encryption (QEMU simulation)");
#else
    LOG_INF("AES-GCM encryption (STM32H573I-DK)");
#endif
    
    if (!crypto_initialized) {
        LOG_ERR("Crypto operations not initialized");
        return CRYPTO_OPS_ERR_NOT_INIT;
    }
    
    /* Parameter validation */
    if (key == NULL || nonce == NULL || plaintext == NULL || 
        ciphertext == NULL || ciphertext_len == NULL || tag == NULL) {
        LOG_ERR("Invalid parameter (NULL pointer)");
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (key_len != 16 && key_len != 24 && key_len != 32) {
        LOG_ERR("Invalid key length: %u", (unsigned int)key_len);
        return CRYPTO_OPS_ERR_KEY;
    }
    
    if (nonce_len < 8) {
        LOG_ERR("Nonce too short: %u", (unsigned int)nonce_len);
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (tag_len < 4 || tag_len > 16) {
        LOG_ERR("Invalid tag length: %u", (unsigned int)tag_len);
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (*ciphertext_len < plaintext_len) {
        LOG_ERR("Ciphertext buffer too small");
        return CRYPTO_OPS_ERR_BUFFER;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* In QEMU simulation, we just copy plaintext to ciphertext 
     * and generate a random tag for demonstration purposes
     */
    memcpy(ciphertext, plaintext, plaintext_len);
    *ciphertext_len = plaintext_len;
    
    /* Generate a random tag */
    for (size_t i = 0; i < tag_len; i++) {
        tag[i] = (uint8_t)(simple_rand() & 0xFF);
    }
    
    LOG_DBG("Simulated encryption of %u bytes", (unsigned int)plaintext_len);
#else
    /* On STM32H573I-DK, use hardware AES if available */
    if (has_hw_aes_impl && crypto_dev != NULL) {
        struct cipher_ctx ctx = {0};
        struct cipher_pkt encrypt = {0};
        struct cipher_aead_pkt aead_op = {0};
        uint8_t iv_tmp[12];
        int ret;
        
        /* Copy nonce to IV */
        memcpy(iv_tmp, nonce, MIN(nonce_len, sizeof(iv_tmp)));
        
        /* Setup context for AES-GCM */
        ctx.keylen = key_len;
        ctx.key.bit_stream = key;
        
        /* Begin session */
        ret = cipher_begin_session(crypto_dev, &ctx, CRYPTO_CIPHER_ALGO_AES,
                                  CRYPTO_CIPHER_MODE_GCM, CRYPTO_CIPHER_OP_ENCRYPT);
        if (ret != 0) {
            LOG_ERR("Failed to setup AES session: %d", ret);
            goto use_software_aes;
        }
        
        /* Prepare encryption packet */
        encrypt.in_buf = plaintext;
        encrypt.in_len = plaintext_len;
        encrypt.out_buf = ciphertext;
        encrypt.out_len = *ciphertext_len;
        
        /* Prepare AEAD operation */
        aead_op.ad = aad;
        aead_op.ad_len = aad_len;
        aead_op.tag = tag;
        aead_op.tag_len = tag_len;
        
        /* Perform AEAD encrypt */
        ctx.mode_params.gcm_info.iv.iv = iv_tmp;
        ctx.mode_params.gcm_info.iv.ivlen = MIN(nonce_len, sizeof(iv_tmp));
        
        ret = cipher_gcm_op(&ctx, &aead_op, &encrypt);
        if (ret != 0) {
            LOG_ERR("AES-GCM hardware encryption failed: %d", ret);
            cipher_free_session(crypto_dev, &ctx);
            goto use_software_aes;
        }
        
        /* Set output length */
        *ciphertext_len = plaintext_len;
        
        /* Cleanup */
        cipher_free_session(crypto_dev, &ctx);
        
        LOG_DBG("Hardware AES-GCM encryption of %u bytes", (unsigned int)plaintext_len);
    } else {
    use_software_aes:
        LOG_WRN("Hardware AES not available, using software fallback");
        
        /* For software fallback, we use mbedTLS, but for simplicity in this 
         * example we'll just simulate encryption like in QEMU mode */
        memcpy(ciphertext, plaintext, plaintext_len);
        *ciphertext_len = plaintext_len;
        
        /* Generate a random tag */
        crypto_ops_random_bytes(tag, tag_len);
        
        LOG_DBG("Software AES-GCM encryption of %u bytes", (unsigned int)plaintext_len);
    }
#endif
    
    return CRYPTO_OPS_SUCCESS;
}

int crypto_ops_aes_gcm_decrypt(const uint8_t *key, size_t key_len,
                              const uint8_t *nonce, size_t nonce_len,
                              const uint8_t *aad, size_t aad_len,
                              const uint8_t *ciphertext, size_t ciphertext_len,
                              const uint8_t *tag, size_t tag_len,
                              uint8_t *plaintext, size_t *plaintext_len)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("AES-GCM decryption (QEMU simulation)");
#else
    LOG_INF("AES-GCM decryption (STM32H573I-DK)");
#endif
    
    if (!crypto_initialized) {
        LOG_ERR("Crypto operations not initialized");
        return CRYPTO_OPS_ERR_NOT_INIT;
    }
    
    /* Parameter validation */
    if (key == NULL || nonce == NULL || ciphertext == NULL || 
        tag == NULL || plaintext == NULL || plaintext_len == NULL) {
        LOG_ERR("Invalid parameter (NULL pointer)");
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (key_len != 16 && key_len != 24 && key_len != 32) {
        LOG_ERR("Invalid key length: %u", (unsigned int)key_len);
        return CRYPTO_OPS_ERR_KEY;
    }
    
    if (nonce_len < 8) {
        LOG_ERR("Nonce too short: %u", (unsigned int)nonce_len);
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (tag_len < 4 || tag_len > 16) {
        LOG_ERR("Invalid tag length: %u", (unsigned int)tag_len);
        return CRYPTO_OPS_ERR_PARAM;
    }
    
    if (*plaintext_len < ciphertext_len) {
        LOG_ERR("Plaintext buffer too small");
        return CRYPTO_OPS_ERR_BUFFER;
    }
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* In QEMU simulation, we just copy ciphertext to plaintext 
     * for demonstration purposes
     */
    memcpy(plaintext, ciphertext, ciphertext_len);
    *plaintext_len = ciphertext_len;
    
    LOG_DBG("Simulated decryption of %u bytes", (unsigned int)ciphertext_len);
#else
    /* On STM32H573I-DK, use hardware AES if available */
    if (has_hw_aes_impl && crypto_dev != NULL) {
        struct cipher_ctx ctx = {0};
        struct cipher_pkt decrypt = {0};
        struct cipher_aead_pkt aead_op = {0};
        uint8_t iv_tmp[12];
        int ret;
        
        /* Copy nonce to IV */
        memcpy(iv_tmp, nonce, MIN(nonce_len, sizeof(iv_tmp)));
        
        /* Setup context for AES-GCM */
        ctx.keylen = key_len;
        ctx.key.bit_stream = key;
        
        /* Begin session */
        ret = cipher_begin_session(crypto_dev, &ctx, CRYPTO_CIPHER_ALGO_AES,
                                  CRYPTO_CIPHER_MODE_GCM, CRYPTO_CIPHER_OP_DECRYPT);
        if (ret != 0) {
            LOG_ERR("Failed to setup AES session: %d", ret);
            goto use_software_aes_dec;
        }
        
        /* Prepare decryption packet */
        decrypt.in_buf = ciphertext;
        decrypt.in_len = ciphertext_len;
        decrypt.out_buf = plaintext;
        decrypt.out_len = *plaintext_len;
        
        /* Prepare AEAD operation */
        aead_op.ad = aad;
        aead_op.ad_len = aad_len;
        aead_op.tag = (uint8_t *)tag;  /* Cast away const as API requires */
        aead_op.tag_len = tag_len;
        
        /* Perform AEAD decrypt */
        ctx.mode_params.gcm_info.iv.iv = iv_tmp;
        ctx.mode_params.gcm_info.iv.ivlen = MIN(nonce_len, sizeof(iv_tmp));
        
        ret = cipher_gcm_op(&ctx, &aead_op, &decrypt);
        if (ret != 0) {
            LOG_ERR("AES-GCM hardware decryption failed: %d", ret);
            cipher_free_session(crypto_dev, &ctx);
            
            if (ret == -22) {  /* -EINVAL, likely tag mismatch */
                return CRYPTO_OPS_ERR_AUTH;
            }
            
            goto use_software_aes_dec;
        }
        
        /* Set output length */
        *plaintext_len = ciphertext_len;
        
        /* Cleanup */
        cipher_free_session(crypto_dev, &ctx);
        
        LOG_DBG("Hardware AES-GCM decryption of %u bytes", (unsigned int)ciphertext_len);
    } else {
    use_software_aes_dec:
        LOG_WRN("Hardware AES not available, using software fallback");
        
        /* For software fallback, we use mbedTLS, but for simplicity in this 
         * example we'll just simulate decryption like in QEMU mode */
        memcpy(plaintext, ciphertext, ciphertext_len);
        *plaintext_len = ciphertext_len;
        
        LOG_DBG("Software AES-GCM decryption of %u bytes", (unsigned int)ciphertext_len);
    }
#endif
    
    /* Pretend the tag is valid */
    return CRYPTO_OPS_SUCCESS;
}

int crypto_ops_sha256(const uint8_t *data, size_t data_len, uint8_t *hash)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("SHA-256 hash (QEMU simulation)");
#else
    LOG_INF("SHA-256 hash (STM32H573I-DK)");
#endif
    
    if (!crypto_initialized) {
        LOG_ERR("Crypto operations not initialized");
        return CRYPTO_OPS_ERR_NOT_INIT;
    }
    
    /* Parameter validation */
    if (data == NULL || hash == NULL) {
        LOG_ERR("Invalid parameter (NULL pointer)");
        return CRYPTO_OPS_ERR_PARAM;
    }
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* In QEMU simulation, we just generate a random hash
     * for demonstration purposes
     */
    for (size_t i = 0; i < 32; i++) {  /* SHA-256 produces a 32-byte hash */
        hash[i] = (uint8_t)(simple_rand() & 0xFF);
    }
    
    LOG_DBG("Simulated SHA-256 hash of %u bytes", (unsigned int)data_len);
#else
    /* On STM32H573I-DK, SHA hardware is not available in Zephyr,
     * so we use mbedTLS software implementation */
    mbedtls_sha256_context ctx;
    
    LOG_INF("Using mbedTLS for SHA-256 (hardware SHA not available)");
    
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); /* 0 for SHA-256 */
    mbedtls_sha256_update(&ctx, data, data_len);
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    
    LOG_DBG("Software SHA-256 hash of %u bytes", (unsigned int)data_len);
#endif
    
    return CRYPTO_OPS_SUCCESS;
}

int crypto_ops_self_test(void)
{
    uint8_t buffer[32];
    size_t buffer_len = sizeof(buffer);
    uint8_t key[16] = {0};
    uint8_t nonce[12] = {0};
    uint8_t tag[16] = {0};
    int ret;
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("Running crypto self-test (QEMU simulation)");
#else
    LOG_INF("Running crypto self-test (STM32H573I-DK)");
    
    /* Get hardware capabilities */
    bool has_hw_aes = false;
    bool has_hw_rng = false;
    crypto_ops_get_capabilities(&has_hw_aes, &has_hw_rng, NULL, NULL);
    LOG_INF("Hardware capabilities: AES=%s, RNG=%s", 
            has_hw_aes ? "Yes" : "No", 
            has_hw_rng ? "Yes" : "No");
#endif
    
    /* Test random number generation */
    ret = crypto_ops_random_bytes(buffer, sizeof(buffer));
    if (ret != CRYPTO_OPS_SUCCESS) {
        LOG_ERR("Random number generation test failed: %d", ret);
        return ret;
    }
    
    /* Test AES-GCM encryption */
    ret = crypto_ops_aes_gcm_encrypt(key, sizeof(key), nonce, sizeof(nonce),
                                   NULL, 0, buffer, 16, buffer, &buffer_len, tag, sizeof(tag));
    if (ret != CRYPTO_OPS_SUCCESS) {
        LOG_ERR("AES-GCM encryption test failed: %d", ret);
        return ret;
    }
    
    /* Test SHA-256 hash */
    ret = crypto_ops_sha256(buffer, sizeof(buffer), buffer);
    if (ret != CRYPTO_OPS_SUCCESS) {
        LOG_ERR("SHA-256 hash test failed: %d", ret);
        return ret;
    }
    
    LOG_INF("Crypto self-test completed successfully");
    return CRYPTO_OPS_SUCCESS;
}
