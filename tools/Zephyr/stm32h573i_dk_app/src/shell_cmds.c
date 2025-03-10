/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "shell_cmds.h"
#include "crypto_ops.h"

LOG_MODULE_REGISTER(shell_cmds, CONFIG_LOG_DEFAULT_LEVEL);

/* Helper functions for converting between hex strings and binary data */
static int hex_char_to_nibble(char c, uint8_t *nibble)
{
    if (c >= '0' && c <= '9') {
        *nibble = c - '0';
        return 0;
    } else if (c >= 'a' && c <= 'f') {
        *nibble = c - 'a' + 10;
        return 0;
    } else if (c >= 'A' && c <= 'F') {
        *nibble = c - 'A' + 10;
        return 0;
    }
    return -1;
}

static int hex_string_to_bytes(const char *hex_str, uint8_t *bytes, size_t max_len)
{
    size_t hex_len = strlen(hex_str);
    size_t byte_len = hex_len / 2;
    
    if (hex_len % 2 != 0) {
        return -1; /* Hex string must have even length */
    }
    
    if (byte_len > max_len) {
        return -1; /* Output buffer too small */
    }
    
    for (size_t i = 0; i < byte_len; i++) {
        uint8_t msn, lsn;
        
        if (hex_char_to_nibble(hex_str[i * 2], &msn) != 0) {
            return -1;
        }
        
        if (hex_char_to_nibble(hex_str[i * 2 + 1], &lsn) != 0) {
            return -1;
        }
        
        bytes[i] = (msn << 4) | lsn;
    }
    
    return byte_len;
}

static void bytes_to_hex_string(const uint8_t *bytes, size_t len, char *hex_str, size_t hex_str_len)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    size_t i, j;
    
    for (i = 0, j = 0; i < len && j < hex_str_len - 2; i++) {
        hex_str[j++] = hex_chars[(bytes[i] >> 4) & 0xF];
        hex_str[j++] = hex_chars[bytes[i] & 0xF];
    }
    
    hex_str[j] = '\0';
}

/* Command handlers */
static int cmd_crypto_status(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    bool has_hw_aes = false;
    bool has_hw_rng = false;
    bool has_hw_sha = false;
    bool has_hw_pka = false;
    
    crypto_ops_get_capabilities(&has_hw_aes, &has_hw_rng, &has_hw_sha, &has_hw_pka);
    
    shell_print(sh, "Crypto hardware capabilities:");
    shell_print(sh, "  AES hardware acceleration: %s", has_hw_aes ? "Available" : "Not available");
    shell_print(sh, "  Random number generator:   %s", has_hw_rng ? "Hardware" : "Software");
    shell_print(sh, "  SHA hardware acceleration: %s", has_hw_sha ? "Available" : "Not available");
    shell_print(sh, "  Public key accelerator:    %s", has_hw_pka ? "Available" : "Not available");
    
    return 0;
}

static int cmd_crypto_selftest(const struct shell *sh, size_t argc, char **argv)
{
    ARG_UNUSED(argc);
    ARG_UNUSED(argv);
    
    int ret = crypto_ops_self_test();
    
    if (ret == 0) {
        shell_print(sh, "Crypto self-test completed successfully");
    } else {
        shell_error(sh, "Crypto self-test failed: %d", ret);
    }
    
    return 0;
}

static int cmd_crypto_random(const struct shell *sh, size_t argc, char **argv)
{
    size_t len = 16;  /* Default to 16 bytes */
    uint8_t buffer[64];
    char hex_str[129];  /* 64 bytes * 2 chars per byte + null terminator */
    int ret;
    
    if (argc > 1) {
        len = atoi(argv[1]);
        if (len == 0 || len > sizeof(buffer)) {
            shell_error(sh, "Invalid length. Must be between 1 and %u", sizeof(buffer));
            return -1;
        }
    }
    
    ret = crypto_ops_random_bytes(buffer, len);
    if (ret != 0) {
        shell_error(sh, "Failed to generate random bytes: %d", ret);
        return -1;
    }
    
    bytes_to_hex_string(buffer, len, hex_str, sizeof(hex_str));
    shell_print(sh, "Random bytes (%u): %s", (unsigned int)len, hex_str);
    
    return 0;
}

static int cmd_crypto_hash_sha256(const struct shell *sh, size_t argc, char **argv)
{
    uint8_t hash[32];
    char hex_str[65];  /* 32 bytes * 2 chars per byte + null terminator */
    int ret;
    
    if (argc != 2) {
        shell_error(sh, "Usage: crypto hash sha256 <data_in_hex>");
        return -1;
    }
    
    ret = crypto_ops_sha256((const uint8_t *)argv[1], strlen(argv[1]), hash);
    if (ret != 0) {
        shell_error(sh, "Failed to compute SHA-256 hash: %d", ret);
        return -1;
    }
    
    bytes_to_hex_string(hash, sizeof(hash), hex_str, sizeof(hex_str));
    shell_print(sh, "SHA-256 hash: %s", hex_str);
    
    return 0;
}

static int cmd_crypto_encrypt_aes_gcm(const struct shell *sh, size_t argc, char **argv)
{
    uint8_t key[32];
    uint8_t nonce[12];
    uint8_t plaintext[256];
    uint8_t ciphertext[256];
    uint8_t tag[16];
    size_t key_len, nonce_len, plaintext_len, ciphertext_len = sizeof(ciphertext);
    char hex_str[513];  /* 256 bytes * 2 chars per byte + null terminator */
    char tag_hex[33];   /* 16 bytes * 2 chars per byte + null terminator */
    int ret;
    
    if (argc != 4) {
        shell_error(sh, "Usage: crypto encrypt aes_gcm <key_hex> <nonce_hex> <plaintext_hex>");
        return -1;
    }
    
    key_len = hex_string_to_bytes(argv[1], key, sizeof(key));
    if (key_len <= 0 || (key_len != 16 && key_len != 24 && key_len != 32)) {
        shell_error(sh, "Invalid key. Must be 16, 24, or 32 bytes (32, 48, or 64 hex chars)");
        return -1;
    }
    
    nonce_len = hex_string_to_bytes(argv[2], nonce, sizeof(nonce));
    if (nonce_len <= 0 || nonce_len > sizeof(nonce)) {
        shell_error(sh, "Invalid nonce. Must be up to 12 bytes (24 hex chars)");
        return -1;
    }
    
    plaintext_len = hex_string_to_bytes(argv[3], plaintext, sizeof(plaintext));
    if (plaintext_len <= 0) {
        shell_error(sh, "Invalid plaintext");
        return -1;
    }
    
    ret = crypto_ops_aes_gcm_encrypt(key, key_len, nonce, nonce_len, NULL, 0,
                                    plaintext, plaintext_len, ciphertext, &ciphertext_len,
                                    tag, sizeof(tag));
    if (ret != 0) {
        shell_error(sh, "Encryption failed: %d", ret);
        return -1;
    }
    
    bytes_to_hex_string(ciphertext, ciphertext_len, hex_str, sizeof(hex_str));
    bytes_to_hex_string(tag, sizeof(tag), tag_hex, sizeof(tag_hex));
    
    shell_print(sh, "Ciphertext: %s", hex_str);
    shell_print(sh, "Auth tag:   %s", tag_hex);
    
    return 0;
}

static int cmd_crypto_decrypt_aes_gcm(const struct shell *sh, size_t argc, char **argv)
{
    uint8_t key[32];
    uint8_t nonce[12];
    uint8_t ciphertext[256];
    uint8_t tag[16];
    uint8_t plaintext[256];
    size_t key_len, nonce_len, ciphertext_len, tag_len, plaintext_len = sizeof(plaintext);
    char hex_str[513];  /* 256 bytes * 2 chars per byte + null terminator */
    int ret;
    
    if (argc != 5) {
        shell_error(sh, "Usage: crypto decrypt aes_gcm <key_hex> <nonce_hex> <ciphertext_hex> <tag_hex>");
        return -1;
    }
    
    key_len = hex_string_to_bytes(argv[1], key, sizeof(key));
    if (key_len <= 0 || (key_len != 16 && key_len != 24 && key_len != 32)) {
        shell_error(sh, "Invalid key. Must be 16, 24, or 32 bytes (32, 48, or 64 hex chars)");
        return -1;
    }
    
    nonce_len = hex_string_to_bytes(argv[2], nonce, sizeof(nonce));
    if (nonce_len <= 0 || nonce_len > sizeof(nonce)) {
        shell_error(sh, "Invalid nonce. Must be up to 12 bytes (24 hex chars)");
        return -1;
    }
    
    ciphertext_len = hex_string_to_bytes(argv[3], ciphertext, sizeof(ciphertext));
    if (ciphertext_len <= 0) {
        shell_error(sh, "Invalid ciphertext");
        return -1;
    }
    
    tag_len = hex_string_to_bytes(argv[4], tag, sizeof(tag));
    if (tag_len <= 0 || tag_len > sizeof(tag)) {
        shell_error(sh, "Invalid tag. Must be up to 16 bytes (32 hex chars)");
        return -1;
    }
    
    ret = crypto_ops_aes_gcm_decrypt(key, key_len, nonce, nonce_len, NULL, 0,
                                    ciphertext, ciphertext_len, tag, tag_len,
                                    plaintext, &plaintext_len);
    if (ret != 0) {
        shell_error(sh, "Decryption failed: %d", ret);
        return -1;
    }
    
    bytes_to_hex_string(plaintext, plaintext_len, hex_str, sizeof(hex_str));
    shell_print(sh, "Plaintext: %s", hex_str);
    
    return 0;
}

/* Create subcommand sets */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_crypto_hash,
    SHELL_CMD_ARG(sha256, NULL, "Compute SHA-256 hash", cmd_crypto_hash_sha256, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_crypto_encrypt,
    SHELL_CMD_ARG(aes_gcm, NULL, "AES-GCM encryption", cmd_crypto_encrypt_aes_gcm, 4, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_crypto_decrypt,
    SHELL_CMD_ARG(aes_gcm, NULL, "AES-GCM decryption", cmd_crypto_decrypt_aes_gcm, 5, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_STATIC_SUBCMD_SET_CREATE(sub_crypto,
    SHELL_CMD_ARG(status, NULL, "Show crypto hardware status", cmd_crypto_status, 1, 0),
    SHELL_CMD_ARG(selftest, NULL, "Run crypto self-test", cmd_crypto_selftest, 1, 0),
    SHELL_CMD_ARG(random, NULL, "Generate random bytes: random [length]", cmd_crypto_random, 1, 1),
    SHELL_CMD(hash, &sub_crypto_hash, "Hash functions", NULL),
    SHELL_CMD(encrypt, &sub_crypto_encrypt, "Encryption functions", NULL),
    SHELL_CMD(decrypt, &sub_crypto_decrypt, "Decryption functions", NULL),
    SHELL_SUBCMD_SET_END
);

/* Register the main "crypto" command */
SHELL_CMD_REGISTER(crypto, &sub_crypto, "Cryptographic operations", NULL);

int shell_cmds_init(void)
{
    LOG_INF("Shell commands initialized");
    return 0;
}
