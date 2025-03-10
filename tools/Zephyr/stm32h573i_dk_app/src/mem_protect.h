/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MEM_PROTECT_H
#define MEM_PROTECT_H

#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>

/** Error codes for memory protection operations */
#define MEM_PROTECT_SUCCESS         0
#define MEM_PROTECT_ERROR_INIT     -1
#define MEM_PROTECT_ERROR_REGION   -2
#define MEM_PROTECT_ERROR_ACCESS   -3
#define MEM_PROTECT_ERROR_CONFIG   -4

/** Memory region types */
typedef enum {
    MEM_REGION_SECURE_DATA,    /**< Region for storing sensitive crypto data (keys, etc.) */
    MEM_REGION_CRYPTO_BUFFER,  /**< Region for temporary crypto operation buffers */
    MEM_REGION_CODE,           /**< Region for secure code execution */
    MEM_REGION_PERIPHERAL,     /**< Region for secure peripherals */
    MEM_REGION_COUNT           /**< Number of region types */
} mem_region_type_t;

/** Memory region access permissions */
typedef enum {
    MEM_ACCESS_READ_ONLY,      /**< Read-only access */
    MEM_ACCESS_READ_WRITE,     /**< Read-write access */
    MEM_ACCESS_READ_EXECUTE,   /**< Read-execute access (for code) */
    MEM_ACCESS_NO_ACCESS       /**< No access (protected) */
} mem_access_perm_t;

/**
 * @brief Initialize the memory protection system
 *
 * Sets up memory protection regions and configures the MPU for secure operation.
 *
 * @return MEM_PROTECT_SUCCESS on success, error code on failure
 */
int mem_protect_init(void);

/**
 * @brief Configure a memory region with specific access permissions
 *
 * @param region_type Type of memory region to configure
 * @param access_perm Access permissions to apply
 * @return MEM_PROTECT_SUCCESS on success, error code on failure
 */
int mem_protect_configure_region(mem_region_type_t region_type, mem_access_perm_t access_perm);

/**
 * @brief Allocate memory from the secure memory region
 *
 * This function allocates memory from the secure region with specific 
 * alignment requirements. The allocated memory is protected from unauthorized access.
 *
 * @param size Size of memory to allocate in bytes
 * @param align Alignment requirement (must be power of 2)
 * @return Pointer to allocated memory or NULL on failure
 */
void *mem_protect_alloc_secure(size_t size, size_t align);

/**
 * @brief Free memory allocated from the secure memory region
 *
 * @param ptr Pointer to memory previously allocated with mem_protect_alloc_secure
 */
void mem_protect_free_secure(void *ptr);

/**
 * @brief Sanitize (zero) secure memory region
 *
 * This function should be called before releasing a secure memory region
 * to ensure all sensitive data is cleared.
 *
 * @param ptr Pointer to memory region to sanitize
 * @param size Size of memory region in bytes
 */
void mem_protect_sanitize(void *ptr, size_t size);

/**
 * @brief Check if memory protection is active
 *
 * @return true if memory protection is active, false otherwise
 */
bool mem_protect_is_active(void);

/**
 * @brief Check if a memory region is secure
 *
 * @param ptr Pointer to memory to check
 * @param size Size of memory region in bytes
 * @return true if the entire memory region is secure, false otherwise
 */
bool mem_protect_is_secure_region(void *ptr, size_t size);

#endif /* MEM_PROTECT_H */
