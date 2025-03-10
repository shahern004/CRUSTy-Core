/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <string.h>
#include "mem_protect.h"
#include "mem_protect_demo.h"
#include "uart_comm.h"
#include <zephyr/sys/printk.h>

/* Log module declaration */
LOG_MODULE_REGISTER(mem_protect_demo, CONFIG_LOG_DEFAULT_LEVEL);

/* Test buffer sizes */
#define SECURE_KEY_SIZE         32
#define SECURE_BUFFER_SIZE      256
#define TEST_PATTERN_SIZE       16

/* Test data */
static const uint8_t test_pattern[TEST_PATTERN_SIZE] = {
    0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
    0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
};

/* Global tracking variables */
static bool demo_initialized = false;
static uint8_t *secure_key = NULL;
static uint8_t *secure_buffer = NULL;

int mem_protect_demo_init(void)
{
    int ret;
    
    LOG_INF("Initializing memory protection demo...");
    
    /* Initialize memory protection if not already done */
    ret = mem_protect_init();
    if (ret != MEM_PROTECT_SUCCESS) {
        LOG_ERR("Failed to initialize memory protection: %d", ret);
        return -1;
    }
    
    LOG_INF("Memory protection initialized");
    
    /* Check if memory protection is active */
    if (!mem_protect_is_active()) {
        LOG_ERR("Memory protection did not activate properly");
        return -1;
    }
    
    demo_initialized = true;
    LOG_INF("Memory protection demo initialized successfully");
    
    return 0;
}

/**
 * @brief Test allocating and using secure memory
 * 
 * @return 0 on success, error code on failure
 */
static int test_secure_allocation(void)
{
    LOG_INF("Testing secure memory allocation...");
    
    /* Allocate memory for a cryptographic key */
    secure_key = mem_protect_alloc_secure(SECURE_KEY_SIZE, 8);
    if (secure_key == NULL) {
        LOG_ERR("Failed to allocate secure memory for key");
        return -1;
    }
    
    /* Verify the memory is secure */
    if (!mem_protect_is_secure_region(secure_key, SECURE_KEY_SIZE)) {
        LOG_ERR("Allocated memory is not in secure region");
        return -2;
    }
    
    /* Fill with test pattern */
    memcpy(secure_key, test_pattern, MIN(SECURE_KEY_SIZE, TEST_PATTERN_SIZE));
    
    /* Allocate a larger buffer */
    secure_buffer = mem_protect_alloc_secure(SECURE_BUFFER_SIZE, 16);
    if (secure_buffer == NULL) {
        LOG_ERR("Failed to allocate secure buffer");
        return -3;
    }
    
    /* Fill buffer with a pattern */
    for (int i = 0; i < SECURE_BUFFER_SIZE; i++) {
        secure_buffer[i] = (uint8_t)(i & 0xFF);
    }
    
    LOG_INF("Successfully allocated and accessed secure memory");
    return 0;
}

/**
 * @brief Test memory sanitization
 * 
 * @return 0 on success, error code on failure
 */
static int test_memory_sanitization(void)
{
    LOG_INF("Testing memory sanitization...");
    
    if (secure_buffer == NULL) {
        LOG_ERR("Secure buffer not allocated");
        return -1;
    }
    
    /* Fill buffer with a pattern */
    memset(secure_buffer, 0xAA, SECURE_BUFFER_SIZE);
    
    /* Verify pattern was written */
    for (int i = 0; i < SECURE_BUFFER_SIZE; i++) {
        if (secure_buffer[i] != 0xAA) {
            LOG_ERR("Buffer verification failed at index %d", i);
            return -2;
        }
    }
    
    /* Sanitize memory */
    mem_protect_sanitize(secure_buffer, SECURE_BUFFER_SIZE);
    
    /* Verify memory was cleared */
    for (int i = 0; i < SECURE_BUFFER_SIZE; i++) {
        if (secure_buffer[i] != 0) {
            LOG_ERR("Sanitization failed at index %d", i);
            return -3;
        }
    }
    
    LOG_INF("Memory sanitization successful");
    return 0;
}

/**
 * @brief Test memory access control
 * 
 * This function attempts to configure regions with different access permissions
 * 
 * @return 0 on success, error code on failure
 */
static int test_memory_permissions(void)
{
    int ret;
    
    LOG_INF("Testing memory access permissions...");
    
    /* Change secure data region to read-only */
    ret = mem_protect_configure_region(MEM_REGION_SECURE_DATA, MEM_ACCESS_READ_ONLY);
    if (ret != MEM_PROTECT_SUCCESS) {
        LOG_ERR("Failed to configure secure data region as read-only: %d", ret);
        return -1;
    }
    
    /* Note: On actual hardware, writing to this region would cause a memory protection fault,
     * but we can't easily test that in software without causing a crash. In a real implementation,
     * we would use special testing code that catches and reports MPU faults.
     */
    
    /* Restore read-write access */
    ret = mem_protect_configure_region(MEM_REGION_SECURE_DATA, MEM_ACCESS_READ_WRITE);
    if (ret != MEM_PROTECT_SUCCESS) {
        LOG_ERR("Failed to restore secure data region permissions: %d", ret);
        return -2;
    }
    
    LOG_INF("Memory permissions testing complete");
    return 0;
}

/**
 * @brief Clean up resources
 */
static void cleanup_resources(void)
{
    LOG_INF("Cleaning up resources...");
    
    /* Free secure memory */
    if (secure_key != NULL) {
        mem_protect_free_secure(secure_key);
        secure_key = NULL;
    }
    
    if (secure_buffer != NULL) {
        mem_protect_free_secure(secure_buffer);
        secure_buffer = NULL;
    }
    
    LOG_INF("Resources cleaned up");
}

int mem_protect_demo_run(void)
{
    int ret;
    char buffer[80];
    
    if (!demo_initialized) {
        LOG_ERR("Memory protection demo not initialized");
        return -1;
    }
    
    LOG_INF("Starting memory protection demo...");
    printk("==== MEMORY PROTECTION DEMO ====\r\n");
    
    /* Use uart_comm_send if it's available */
    if (uart_comm_is_ready()) {
        const char *msg = "==== MEMORY PROTECTION DEMO ====\r\n";
        uart_comm_send((const uint8_t *)msg, strlen(msg), K_FOREVER);
    }
    
    /* Step 1: Test secure memory allocation */
    ret = test_secure_allocation();
    if (ret != 0) {
        LOG_ERR("Secure allocation test failed: %d", ret);
        sprintf(buffer, "Secure allocation test FAILED: %d\r\n", ret);
        printk("%s", buffer);
        
        if (uart_comm_is_ready()) {
            uart_comm_send((const uint8_t *)buffer, strlen(buffer), K_FOREVER);
        }
        cleanup_resources();
        return -1;
    }
    printk("Secure allocation test PASSED\r\n");
    
    if (uart_comm_is_ready()) {
        const char *msg = "Secure allocation test PASSED\r\n";
        uart_comm_send((const uint8_t *)msg, strlen(msg), K_FOREVER);
    }
    
    /* Step 2: Test memory sanitization */
    ret = test_memory_sanitization();
    if (ret != 0) {
        LOG_ERR("Memory sanitization test failed: %d", ret);
        sprintf(buffer, "Memory sanitization test FAILED: %d\r\n", ret);
        printk("%s", buffer);
        
        if (uart_comm_is_ready()) {
            uart_comm_send((const uint8_t *)buffer, strlen(buffer), K_FOREVER);
        }
        cleanup_resources();
        return -1;
    }
    printk("Memory sanitization test PASSED\r\n");
    
    if (uart_comm_is_ready()) {
        const char *msg = "Memory sanitization test PASSED\r\n";
        uart_comm_send((const uint8_t *)msg, strlen(msg), K_FOREVER);
    }
    
    /* Step 3: Test memory access permissions */
    ret = test_memory_permissions();
    if (ret != 0) {
        LOG_ERR("Memory permissions test failed: %d", ret);
        sprintf(buffer, "Memory permissions test FAILED: %d\r\n", ret);
        printk("%s", buffer);
        
        if (uart_comm_is_ready()) {
            uart_comm_send((const uint8_t *)buffer, strlen(buffer), K_FOREVER);
        }
        cleanup_resources();
        return -1;
    }
    printk("Memory permissions test PASSED\r\n");
    
    if (uart_comm_is_ready()) {
        const char *msg = "Memory permissions test PASSED\r\n";
        uart_comm_send((const uint8_t *)msg, strlen(msg), K_FOREVER);
    }
    
    /* Clean up resources */
    cleanup_resources();
    
    printk("==== MEMORY PROTECTION DEMO COMPLETED SUCCESSFULLY ====\r\n");
    
    if (uart_comm_is_ready()) {
        const char *msg = "==== MEMORY PROTECTION DEMO COMPLETED SUCCESSFULLY ====\r\n";
        uart_comm_send((const uint8_t *)msg, strlen(msg), K_FOREVER);
    }
    LOG_INF("Memory protection demo completed successfully");
    
    return 0;
}

void mem_protect_demo_print_help(void)
{
    printk("\r\n==== MEMORY PROTECTION DEMO HELP ====\r\n");
    printk("This demo shows the memory protection capabilities:\r\n");
    printk("1. Secure memory allocation & protection\r\n");
    printk("2. Memory sanitization\r\n");
    printk("3. Memory access permissions\r\n");
    printk("================================\r\n\r\n");
    
    if (uart_comm_is_ready()) {
        const char *messages[] = {
            "\r\n==== MEMORY PROTECTION DEMO HELP ====\r\n",
            "This demo shows the memory protection capabilities:\r\n",
            "1. Secure memory allocation & protection\r\n",
            "2. Memory sanitization\r\n",
            "3. Memory access permissions\r\n",
            "================================\r\n\r\n"
        };
        
        for (int i = 0; i < sizeof(messages) / sizeof(messages[0]); i++) {
            uart_comm_send((const uint8_t *)messages[i], strlen(messages[i]), K_FOREVER);
        }
    }
}
