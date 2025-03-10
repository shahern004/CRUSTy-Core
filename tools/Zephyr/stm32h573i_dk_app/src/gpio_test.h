/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPIO_TEST_H
#define GPIO_TEST_H

#include <zephyr/kernel.h>

/**
 * @brief Initialize the GPIO test module
 *
 * This function initializes the GPIO test module, setting up the necessary
 * GPIO and UART interfaces for interactive testing.
 *
 * @return 0 on success, negative error code on failure
 */
int gpio_test_init(void);

/**
 * @brief Run the GPIO test sequence
 *
 * This function runs through a predefined test sequence to verify GPIO
 * functionality, including LED control and button simulation.
 *
 * @return 0 on success, negative error code on failure
 */
int gpio_test_run_sequence(void);

/**
 * @brief Process a UART command for GPIO testing
 *
 * This function processes UART commands related to GPIO testing, allowing
 * for interactive control of GPIOs.
 *
 * @param cmd The command string to process
 * @param cmd_len Length of the command string
 * @return 0 on success, negative error code on failure
 */
int gpio_test_process_command(const char *cmd, size_t cmd_len);

/**
 * @brief Print help information for GPIO test commands
 *
 * This function prints help information for the available GPIO test commands
 * to the UART console.
 */
void gpio_test_print_help(void);

/**
 * @brief Simulate a button press
 *
 * This function simulates a button press/release cycle for testing GPIO
 * callbacks without physical hardware.
 *
 * @param duration_ms Duration to hold the button in pressed state, in milliseconds
 * @return 0 on success, negative error code on failure
 */
int gpio_test_simulate_button_press(uint32_t duration_ms);

#endif /* GPIO_TEST_H */
