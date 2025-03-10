/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "crypto_ops.h"
#include "crypto_demo.h"

LOG_MODULE_REGISTER(crypto_demo, CONFIG_LOG_DEFAULT_LEVEL);

/* Test data sizes */
#define DEMO_SMALL_SIZE  64
#define DEMO_MEDIUM_SIZE 512
#define DEMO_LARGE_SIZE  1024

/* AES key, nonce, and AAD */
static uint8_t demo_key[32] = {0};
static uint8_t demo_nonce[12] = {0};
static uint8_t demo_aad[16] = {0};

/* Data buffers */
static uint8_t demo_plaintext[DEMO_LARGE_SIZE] = {0};
static uint8_t demo_ciphertext[DEMO_LARGE_SIZE] = {0};
static uint8_t demo_tag[16] = {0};
static uint8_t demo_decrypted[DEMO_LARGE_SIZE] = {0};
static uint8_t demo_hash[32] = {0};

/* Results */
static bool has_hw_aes = false;
static bool has_hw_rng = false;
static bool has_hw_sha = false;
static bool has_hw_pka = false;

/**
 * @brief Run an AES-GCM demo test with the specified size
 *
 * @param size Size of the test data
 * @return 0 on success, negative value on error
 */
static int run_aes_gcm_test(size_t size)
{
    int ret;
    size_t ciphertext_len = sizeof(demo_ciphertext);
    size_t decrypted_len = sizeof(demo_decrypted);
    uint64_t start_time, encrypt_time, decrypt_time;

    LOG_INF("Running AES-GCM test with %zu bytes", size);

    /* Encrypt the data */
    start_time = k_uptime_get();
    ret = crypto_ops_aes_gcm_encrypt(demo_key, sizeof(demo_key),
                                   demo_nonce, sizeof(demo_nonce),
                                   demo_aad, sizeof(demo_aad),
                                   demo_plaintext, size,
                                   demo_ciphertext, &ciphertext_len,
                                   demo_tag, sizeof(demo_tag));
    encrypt_time = k_uptime_get() - start_time;
    if (ret) {
        LOG_ERR("AES-GCM encryption failed: %d", ret);
        return ret;
    }

    LOG_INF("AES-GCM encryption took %llu ms", encrypt_time);

    /* Decrypt the data */
    start_time = k_uptime_get();
    ret = crypto_ops_aes_gcm_decrypt(demo_key, sizeof(demo_key),
                                   demo_nonce, sizeof(demo_nonce),
                                   demo_aad, sizeof(demo_aad),
                                   demo_ciphertext, ciphertext_len,
                                   demo_tag, sizeof(demo_tag),
                                   demo_decrypted, &decrypted_len);
    decrypt_time = k_uptime_get() - start_time;
    if (ret) {
        LOG_ERR("AES-GCM decryption failed: %d", ret);
        return ret;
    }

    LOG_INF("AES-GCM decryption took %llu ms", decrypt_time);

    /* Verify the decrypted data matches the original */
    if (memcmp(demo_plaintext, demo_decrypted, size) != 0) {
        LOG_ERR("AES-GCM verification failed: decrypted data does not match plaintext");
        return -EINVAL;
    }

    LOG_INF("AES-GCM verification successful");
    LOG_INF("AES-GCM performance: %llu bytes/sec (encryption), %llu bytes/sec (decryption)",
           encrypt_time > 0 ? (size * 1000) / encrypt_time : 0,
           decrypt_time > 0 ? (size * 1000) / decrypt_time : 0);

    return 0;
}

/**
 * @brief Run a SHA-256 demo test with the specified size
 *
 * @param size Size of the test data
 * @return 0 on success, negative value on error
 */
static int run_sha256_test(size_t size)
{
    int ret;
    uint64_t start_time, hash_time;

    LOG_INF("Running SHA-256 test with %zu bytes", size);

    /* Calculate the hash */
    start_time = k_uptime_get();
    ret = crypto_ops_sha256(demo_plaintext, size, demo_hash);
    hash_time = k_uptime_get() - start_time;
    if (ret) {
        LOG_ERR("SHA-256 calculation failed: %d", ret);
        return ret;
    }

    LOG_INF("SHA-256 calculation took %llu ms", hash_time);
    LOG_INF("SHA-256 performance: %llu bytes/sec",
           hash_time > 0 ? (size * 1000) / hash_time : 0);

    /* Print the hash (only first 8 bytes for brevity) */
    LOG_INF("SHA-256 hash (first 8 bytes): %02x %02x %02x %02x %02x %02x %02x %02x",
           demo_hash[0], demo_hash[1], demo_hash[2], demo_hash[3],
           demo_hash[4], demo_hash[5], demo_hash[6], demo_hash[7]);

    return 0;
}

/**
 * @brief Run a random number generation demo test
 *
 * @return 0 on success, negative value on error
 */
static int run_rng_test(void)
{
    int ret;
    uint64_t start_time, rng_time;
    const size_t rng_size = 32;
    uint8_t rand_buf[32];

    LOG_INF("Running RNG test for %zu bytes", rng_size);

    /* Generate random bytes */
    start_time = k_uptime_get();
    ret = crypto_ops_random_bytes(rand_buf, rng_size);
    rng_time = k_uptime_get() - start_time;
    if (ret) {
        LOG_ERR("Random number generation failed: %d", ret);
        return ret;
    }

    LOG_INF("RNG generation took %llu ms", rng_time);
    LOG_INF("RNG performance: %llu bytes/sec",
           rng_time > 0 ? (rng_size * 1000) / rng_time : 0);

    /* Print the random bytes (only first 8 bytes for brevity) */
    LOG_INF("Random data (first 8 bytes): %02x %02x %02x %02x %02x %02x %02x %02x",
           rand_buf[0], rand_buf[1], rand_buf[2], rand_buf[3],
           rand_buf[4], rand_buf[5], rand_buf[6], rand_buf[7]);

    return 0;
}

int crypto_demo_init(void)
{
    int ret;

    LOG_INF("Initializing crypto demo");

    /* Initialize the crypto subsystem */
    ret = crypto_ops_init();
    if (ret) {
        LOG_ERR("Failed to initialize crypto subsystem: %d", ret);
        return ret;
    }

    /* Get capabilities */
    crypto_ops_get_capabilities(&has_hw_aes, &has_hw_rng, &has_hw_sha, &has_hw_pka);
    LOG_INF("Hardware capabilities: AES: %s, RNG: %s, SHA: %s, PKA: %s",
           has_hw_aes ? "YES" : "NO",
           has_hw_rng ? "YES" : "NO",
           has_hw_sha ? "YES" : "NO",
           has_hw_pka ? "YES" : "NO");

    /* Generate random key, nonce, and AAD */
    ret = crypto_ops_random_bytes(demo_key, sizeof(demo_key));
    if (ret) {
        LOG_ERR("Failed to generate random key: %d", ret);
        return ret;
    }

    ret = crypto_ops_random_bytes(demo_nonce, sizeof(demo_nonce));
    if (ret) {
        LOG_ERR("Failed to generate random nonce: %d", ret);
        return ret;
    }

    ret = crypto_ops_random_bytes(demo_aad, sizeof(demo_aad));
    if (ret) {
        LOG_ERR("Failed to generate random AAD: %d", ret);
        return ret;
    }

    /* Generate random plaintext for the largest test */
    ret = crypto_ops_random_bytes(demo_plaintext, sizeof(demo_plaintext));
    if (ret) {
        LOG_ERR("Failed to generate random plaintext: %d", ret);
        return ret;
    }

    LOG_INF("Crypto demo initialized successfully");
    return 0;
}

int crypto_demo_run(void)
{
    int ret;

    LOG_INF("Running crypto demo");

    /* Run the RNG test */
    ret = run_rng_test();
    if (ret) {
        LOG_ERR("RNG test failed: %d", ret);
        return ret;
    }

    /* Run AES-GCM tests with different sizes */
    ret = run_aes_gcm_test(DEMO_SMALL_SIZE);
    if (ret) {
        LOG_ERR("AES-GCM test (small) failed: %d", ret);
        return ret;
    }

    ret = run_aes_gcm_test(DEMO_MEDIUM_SIZE);
    if (ret) {
        LOG_ERR("AES-GCM test (medium) failed: %d", ret);
        return ret;
    }

    ret = run_aes_gcm_test(DEMO_LARGE_SIZE);
    if (ret) {
        LOG_ERR("AES-GCM test (large) failed: %d", ret);
        return ret;
    }

    /* Run SHA-256 tests with different sizes */
    ret = run_sha256_test(DEMO_SMALL_SIZE);
    if (ret) {
        LOG_ERR("SHA-256 test (small) failed: %d", ret);
        return ret;
    }

    ret = run_sha256_test(DEMO_MEDIUM_SIZE);
    if (ret) {
        LOG_ERR("SHA-256 test (medium) failed: %d", ret);
        return ret;
    }

    ret = run_sha256_test(DEMO_LARGE_SIZE);
    if (ret) {
        LOG_ERR("SHA-256 test (large) failed: %d", ret);
        return ret;
    }

    LOG_INF("Crypto demo completed successfully");
    return 0;
}
