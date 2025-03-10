/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "gpio_control.h"
#include "uart_comm.h"
#include "uart_demo.h"
#include "crypto_ops.h"
#include "crypto_demo.h"
#include "gpio_test.h"
#include "mem_protect.h"
#include "mem_protect_demo.h"
#include "shell_cmds.h"

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

/* For QEMU compatibility */
#ifdef QEMU
#define UART_DEV_NAME_QEMU "UART_0"
#define QEMU_TEST_MODE     1  /* Set to 1 to enter GPIO test mode automatically on QEMU */
#endif

/* LED state tracking */
static bool led_blinking[GPIO_LED_COUNT] = {false};
static uint32_t led_blink_periods[GPIO_LED_COUNT] = {0};

/* Button callback */
static void button_pressed_cb(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int state;
    gpio_control_button_get_state(&state);
    
    if (state) {
        LOG_INF("User button pressed");
        
        /* Toggle all LEDs */
        for (int i = 0; i < GPIO_LED_COUNT; i++) {
            if (gpio_control_led_is_ready(i)) {
                if (led_blinking[i]) {
                    gpio_control_led_blink_stop(i);
                    led_blinking[i] = false;
                } else {
                    gpio_control_led_toggle(i);
                }
            }
        }
    } else {
        LOG_INF("User button released");
    }
}

void main(void)
{
    int ret;

    LOG_INF("STM32H573I-DK CRUSTy-Core Demo");
    LOG_INF("Board: %s", CONFIG_BOARD);

#ifdef QEMU
    LOG_INF("Running in QEMU emulation mode");
#else
    LOG_INF("Running on real hardware");
#endif

    /* Initialize GPIO control module */
    ret = gpio_control_init();
    if (ret != GPIO_CTRL_SUCCESS) {
        LOG_ERR("Failed to initialize GPIO control module: %d", ret);
        return;
    }
    LOG_INF("GPIO control module initialized successfully");
    
    /* Check available LEDs */
    LOG_INF("Checking LED availability:");
    LOG_INF("- Red LED: %s", gpio_control_led_is_ready(GPIO_LED_RED) ? "available" : "not available");
    LOG_INF("- Green LED: %s", gpio_control_led_is_ready(GPIO_LED_GREEN) ? "available" : "not available");
    LOG_INF("- Blue LED: %s", gpio_control_led_is_ready(GPIO_LED_BLUE) ? "available" : "not available");
    
    /* Check button availability */
    LOG_INF("User button: %s", gpio_control_button_is_ready() ? "available" : "not available");
    
    /* Register button callback */
    if (gpio_control_button_is_ready()) {
        ret = gpio_control_button_register_callback(button_pressed_cb);
        if (ret != GPIO_CTRL_SUCCESS) {
            LOG_WRN("Failed to register button callback: %d", ret);
        } else {
            LOG_INF("Button callback registered successfully");
        }
    }

    /* Initialize UART demo */
    uart_demo_init();
    LOG_INF("UART demo initialized");

    /* Initialize and run crypto demo */
    ret = crypto_demo_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize crypto demo: %d", ret);
        /* Continue execution even if crypto demo fails */
    } else {
        LOG_INF("Crypto demo initialized successfully");
        
        /* Run the crypto demo - this will perform various crypto operations */
        ret = crypto_demo_run();
        if (ret != 0) {
            LOG_ERR("Crypto demo execution failed: %d", ret);
        } else {
            LOG_INF("Crypto demo executed successfully");
            /* Blink blue LED if available to indicate crypto success */
            if (gpio_control_led_is_ready(GPIO_LED_BLUE)) {
                gpio_control_led_blink(GPIO_LED_BLUE, 500);
                led_blinking[GPIO_LED_BLUE] = true;
                led_blink_periods[GPIO_LED_BLUE] = 500;
            }
        }
    }
    
    /* Initialize and run memory protection demo */
    ret = mem_protect_demo_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize memory protection demo: %d", ret);
        /* Continue execution even if memory protection demo fails */
    } else {
        LOG_INF("Memory protection demo initialized successfully");
        
        /* Run the memory protection demo */
        ret = mem_protect_demo_run();
        if (ret != 0) {
            LOG_ERR("Memory protection demo execution failed: %d", ret);
            /* Blink red LED to indicate failure if available */
            if (gpio_control_led_is_ready(GPIO_LED_RED)) {
                gpio_control_led_blink(GPIO_LED_RED, 250);
                led_blinking[GPIO_LED_RED] = true;
                led_blink_periods[GPIO_LED_RED] = 250;
            }
        } else {
            LOG_INF("Memory protection demo executed successfully");
            /* Faster blink blue LED to indicate memory protection success */
            if (gpio_control_led_is_ready(GPIO_LED_BLUE)) {
                gpio_control_led_blink(GPIO_LED_BLUE, 250);
                led_blinking[GPIO_LED_BLUE] = true;
                led_blink_periods[GPIO_LED_BLUE] = 250;
            }
        }
    }
    
    /* Initialize shell commands */
    ret = shell_cmds_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize shell commands: %d", ret);
    } else {
        LOG_INF("Shell commands initialized successfully");
    }

    /* Blink green LED to indicate system is running */
    if (gpio_control_led_is_ready(GPIO_LED_GREEN)) {
        gpio_control_led_blink(GPIO_LED_GREEN, 1000);
        led_blinking[GPIO_LED_GREEN] = true;
        led_blink_periods[GPIO_LED_GREEN] = 1000;
        LOG_INF("Demo running, Green LED blinking");
    } else if (gpio_control_led_is_ready(GPIO_LED_RED)) {
        /* Fall back to red LED if green isn't available */
        gpio_control_led_blink(GPIO_LED_RED, 1000);
        led_blinking[GPIO_LED_RED] = true;
        led_blink_periods[GPIO_LED_RED] = 1000;
        LOG_INF("Demo running, Red LED blinking");
    }
    
    LOG_INF("Starting main application loop");

#if defined(QEMU) && defined(QEMU_TEST_MODE) && QEMU_TEST_MODE == 1
    /* When running in QEMU, automatically start the GPIO test sequence */
    LOG_INF("QEMU test mode detected, starting GPIO test automatically");
    k_sleep(K_MSEC(1000));  /* Wait for UART setup to complete */
    gpio_test_print_help();
    k_sleep(K_MSEC(500));
    gpio_test_run_sequence();
    
    /* Also show memory protection demo help */
    k_sleep(K_MSEC(500));
    mem_protect_demo_print_help();
#endif

    /* Start UART demo in a separate thread */
    uart_demo_run();
    
    /* We should never reach here as uart_demo_run has an infinite loop */
    LOG_ERR("Unexpected exit from main loop");
}
