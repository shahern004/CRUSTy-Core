/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UART_DEMO_H
#define UART_DEMO_H

#include <zephyr/kernel.h>

/**
 * @brief Initialize the UART demo
 *
 * This function initializes the UART demo, setting up the UART communication
 * with appropriate callback handlers and sending a welcome message.
 */
void uart_demo_init(void);

/**
 * @brief Run the UART demo
 *
 * This function runs the UART demo in an infinite loop, monitoring for
 * button presses and sending status messages. It should be called after
 * uart_demo_init() has been called.
 *
 * Note: This function does not return unless there's an error.
 */
void uart_demo_run(void);

#endif /* UART_DEMO_H */
