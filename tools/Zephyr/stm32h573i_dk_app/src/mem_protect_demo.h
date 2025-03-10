/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef MEM_PROTECT_DEMO_H
#define MEM_PROTECT_DEMO_H

#include <zephyr/kernel.h>

/**
 * @brief Initialize the memory protection demo
 *
 * @return 0 on success, error code on failure
 */
int mem_protect_demo_init(void);

/**
 * @brief Run the memory protection demo
 *
 * This function demonstrates the memory protection capabilities by:
 * 1. Allocating secure memory
 * 2. Testing access permissions
 * 3. Demonstrating memory sanitization
 * 4. Verifying memory protection boundaries
 *
 * @return 0 on success, error code on failure
 */
int mem_protect_demo_run(void);

/**
 * @brief Print help information for the memory protection demo
 */
void mem_protect_demo_print_help(void);

#endif /* MEM_PROTECT_DEMO_H */
