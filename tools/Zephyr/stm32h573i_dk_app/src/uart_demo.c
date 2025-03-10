/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stdlib.h>
#include "uart_comm.h"
#include "gpio_control.h"
#include "uart_demo.h"
#include "gpio_test.h"

LOG_MODULE_REGISTER(uart_demo, CONFIG_LOG_DEFAULT_LEVEL);

/* Defines */
#define UART_DEVICE_NAME "USART1"
#define UART_BAUD_RATE   115200

/* Buffer for demo */
static uint8_t demo_buffer[256];

/* Forward declarations */
static void process_basic_commands(const uint8_t *data, size_t len);

/* UART receive callback function */
static void uart_rx_callback(const uint8_t *data, size_t len)
{
    /* Copy received data to our buffer */
    if (len < sizeof(demo_buffer)) {
        memcpy(demo_buffer, data, len);
        demo_buffer[len] = '\0'; /* Ensure null termination */
        
        /* Log the received data */
        LOG_INF("UART RX (%d bytes): %s", len, demo_buffer);

        /* Try to process as GPIO test command first */
        if (gpio_test_process_command((const char *)data, len) == 0) {
            /* Command was handled by GPIO test module */
            return;
        }
        
        /* Process as basic command if not handled by GPIO test */
        process_basic_commands(data, len);
    } else {
        LOG_WRN("Received data too large for buffer");
    }
}

/**
 * @brief Process basic UART commands
 * 
 * @param data Raw command data
 * @param len Length of the command data
 */
static void process_basic_commands(const uint8_t *data, size_t len)
{
    /* Simple command processing */
    if (strncmp((char *)data, "LED", 3) == 0) {
        /* Format: "LED <led_num> <0|1>" */
        int led_num, state;
        if (sscanf((char *)data, "LED %d %d", &led_num, &state) == 2) {
            /* Control LED based on received command */
            if (state == 0) {
                gpio_control_led_off(led_num);
                uart_comm_send((uint8_t *)"LED OFF\r\n", 9, K_FOREVER);
            } else {
                gpio_control_led_on(led_num);
                uart_comm_send((uint8_t *)"LED ON\r\n", 8, K_FOREVER);
            }
        } else {
            uart_comm_send((uint8_t *)"Invalid LED command format\r\n", 28, K_FOREVER);
        }
    } else if (strncmp((char *)data, "ECHO ", 5) == 0) {
        /* Echo the received data after "ECHO " prefix */
        uart_comm_send((uint8_t *)"Echo: ", 6, K_FOREVER);
        uart_comm_send(data + 5, len - 5, K_FOREVER);
        uart_comm_send((uint8_t *)"\r\n", 2, K_FOREVER);
    } else if (strncmp((char *)data, "HELP", 4) == 0) {
        /* Send help information */
        const char *help_text = 
            "=== Basic Commands ===\r\n"
            "LED <led_num> <0|1> - Control LED (0=off, 1=on)\r\n"
            "ECHO <text> - Echo back the text\r\n"
            "HELP - Show this help text\r\n"
            "\r\n"
            "=== GPIO Test Commands ===\r\n"
            "Type 'gpio help' for GPIO test commands\r\n";
        uart_comm_send((uint8_t *)help_text, strlen(help_text), K_FOREVER);
    } else if (strncmp((char *)data, "gpio", 4) == 0 || strncmp((char *)data, "GPIO", 4) == 0) {
        /* Redirect to GPIO test help */
        gpio_test_print_help();
    } else {
        /* Unrecognized command */
        uart_comm_send((uint8_t *)"Unknown command. Type HELP for available commands.\r\n", 
                      51, K_FOREVER);
    }
}

void uart_demo_init(void)
{
    int ret;
    
    /* Initialize UART communication */
    ret = uart_comm_init(UART_DEVICE_NAME, UART_BAUD_RATE, uart_rx_callback);
    if (ret != UART_COMM_SUCCESS) {
        LOG_ERR("Failed to initialize UART communication: %d", ret);
        return;
    }
    
    /* Initialize GPIO test module */
    ret = gpio_test_init();
    if (ret != 0) {
        LOG_ERR("Failed to initialize GPIO test module: %d", ret);
        /* Continue anyway */
    }
    
    /* Send welcome message */
    const char *welcome_msg = 
        "\r\n\r\n==================================\r\n"
        "CRUSTy-Core UART & GPIO Test Demo\r\n"
        "Type HELP for basic commands\r\n"
        "Type 'gpio help' for GPIO test commands\r\n"
        "==================================\r\n";
    
    uart_comm_send((uint8_t *)welcome_msg, strlen(welcome_msg), K_FOREVER);
    
    LOG_INF("UART demo initialized with GPIO test support");
}

/* Function to run the demo in the background */
void uart_demo_run(void)
{
    /* This is a simple periodic check - in a real application,
     * you might want to have more complex logic here
     */
    while (1) {
        /* Detect button presses for additional demo functionality */
        int button_state = 0;
        if (gpio_control_button_get_state(&button_state) == GPIO_CTRL_SUCCESS && button_state == 1) {
            /* Button 0 pressed - send a status message */
            const char *status_msg = "Button 0 pressed! System status: OK\n";
            uart_comm_send((uint8_t *)status_msg, strlen(status_msg), K_FOREVER);
            
            /* Debounce wait */
            k_msleep(300);
        }
        
        /* Sleep to avoid hogging the CPU */
        k_msleep(100);
    }
}
