/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CRYPTO_DEMO_H_
#define CRYPTO_DEMO_H_

/**
 * @brief Initialize the crypto demo
 * 
 * @return 0 on success, negative value on error
 */
int crypto_demo_init(void);

/**
 * @brief Run the crypto demo test
 * 
 * This function demonstrates the crypto operations by:
 * 1. Generating random data
 * 2. Encrypting the data with AES-GCM
 * 3. Decrypting the data
 * 4. Calculating SHA-256 hash
 * 5. Comparing results and reporting performance metrics
 * 
 * @return 0 on success, negative value on error
 */
int crypto_demo_run(void);

#endif /* CRYPTO_DEMO_H_ */
