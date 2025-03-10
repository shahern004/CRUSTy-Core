/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio_test.h"
#include "gpio_control.h"
#include "uart_comm.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <string.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(gpio_test, CONFIG_LOG_DEFAULT_LEVEL);

/* Private variables */
static struct k_work_delayable button_release_work;
static bool test_running = false;

/* Test sequence specific variables */
static uint8_t current_test_step = 0;
static struct k_timer test_sequence_timer;
static const char *test_step_descriptions[] = {
    "Turn on all LEDs",
    "Turn off all LEDs",
    "Toggle LEDs sequentially",
    "Blink LEDs at different rates",
    "Simulate button press",
    "Test complete"
};

/* For UART communication */
static char uart_buffer[128];

/* Forward declarations of private functions */
static void button_release_work_handler(struct k_work *work);
static void print_led_status(void);
static void handle_led_command(const char *cmd);
static void handle_button_command(const char *cmd);
static void run_led_sequence(void);
static void test_sequence_timer_handler(struct k_timer *timer);

/**
 * @brief Handler for the button release delayed work
 *
 * This simulates releasing the button after a simulated press
 */
static void button_release_work_handler(struct k_work *work)
{
    /* Overwrite the button's GPIO state to simulate release */
    extern const struct gpio_dt_spec button_user;
    
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    if (gpio_control_button_is_ready()) {
        /* Simulate button release by forcing pin value back to inactive state */
        gpio_pin_set_dt(&button_user, 0);
        LOG_INF("Button release simulated");
    }
#endif
}

/**
 * @brief Print the current status of all LEDs
 */
static void print_led_status(void)
{
    const char *led_names[] = {"Red", "Green", "Blue"};
    
    LOG_INF("LED Status:");
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        if (gpio_control_led_is_ready(i)) {
            LOG_INF("- %s LED: Available", led_names[i]);
        } else {
            LOG_INF("- %s LED: Not available", led_names[i]);
        }
    }
}

/**
 * @brief Handle LED control commands
 *
 * @param cmd Command string
 */
static void handle_led_command(const char *cmd)
{
    int led_index = -1;
    
    /* Parse LED index */
    if (strcmp(cmd, "led red") == 0 || strcmp(cmd, "led 0") == 0) {
        led_index = GPIO_LED_RED;
    } else if (strcmp(cmd, "led green") == 0 || strcmp(cmd, "led 1") == 0) {
        led_index = GPIO_LED_GREEN;
    } else if (strcmp(cmd, "led blue") == 0 || strcmp(cmd, "led 2") == 0) {
        led_index = GPIO_LED_BLUE;
    } else if (strncmp(cmd, "led ", 4) == 0) {
        /* If "led" is followed by a number, try to parse it */
        led_index = atoi(cmd + 4);
    }
    
    /* Check if LED index is valid */
    if (led_index < 0 || led_index >= GPIO_LED_COUNT || !gpio_control_led_is_ready(led_index)) {
        LOG_ERR("Invalid LED specified");
        snprintf(uart_buffer, sizeof(uart_buffer), "ERROR: Invalid LED specified\r\n");
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
        return;
    }
    
    /* Handle LED commands */
    if (strstr(cmd, "on")) {
        gpio_control_led_on(led_index);
        LOG_INF("LED %d turned ON", led_index);
        snprintf(uart_buffer, sizeof(uart_buffer), "LED %d turned ON\r\n", led_index);
    } else if (strstr(cmd, "off")) {
        gpio_control_led_off(led_index);
        LOG_INF("LED %d turned OFF", led_index);
        snprintf(uart_buffer, sizeof(uart_buffer), "LED %d turned OFF\r\n", led_index);
    } else if (strstr(cmd, "toggle")) {
        gpio_control_led_toggle(led_index);
        LOG_INF("LED %d toggled", led_index);
        snprintf(uart_buffer, sizeof(uart_buffer), "LED %d toggled\r\n", led_index);
    } else if (strstr(cmd, "blink")) {
        /* Parse blink period if provided */
        uint32_t period = 1000; /* default 1 second */
        const char *period_str = strstr(cmd, "blink");
        if (period_str) {
            period_str += 5; /* skip "blink" */
            while (*period_str == ' ') period_str++; /* skip spaces */
            if (*period_str) {
                period = atoi(period_str);
                if (period < 100) period = 100; /* minimum period */
                if (period > 10000) period = 10000; /* maximum period */
            }
        }
        
        gpio_control_led_blink(led_index, period);
        LOG_INF("LED %d blinking with period %d ms", led_index, period);
        snprintf(uart_buffer, sizeof(uart_buffer), "LED %d blinking with period %d ms\r\n", led_index, period);
    } else if (strstr(cmd, "stop")) {
        gpio_control_led_blink_stop(led_index);
        LOG_INF("LED %d blinking stopped", led_index);
        snprintf(uart_buffer, sizeof(uart_buffer), "LED %d blinking stopped\r\n", led_index);
    } else {
        LOG_WRN("Unknown LED command");
        snprintf(uart_buffer, sizeof(uart_buffer), "ERROR: Unknown LED command\r\n");
    }
    
    uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
}

/**
 * @brief Handle button simulation commands
 *
 * @param cmd Command string
 */
static void handle_button_command(const char *cmd)
{
    if (strstr(cmd, "press")) {
        /* Parse press duration if provided */
        uint32_t duration = 500; /* default 500ms */
        const char *duration_str = strstr(cmd, "press");
        if (duration_str) {
            duration_str += 5; /* skip "press" */
            while (*duration_str == ' ') duration_str++; /* skip spaces */
            if (*duration_str) {
                duration = atoi(duration_str);
                if (duration < 100) duration = 100; /* minimum duration */
                if (duration > 5000) duration = 5000; /* maximum duration */
            }
        }
        
        gpio_test_simulate_button_press(duration);
        LOG_INF("Button press simulated with duration %d ms", duration);
        snprintf(uart_buffer, sizeof(uart_buffer), "Button press simulated with duration %d ms\r\n", duration);
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
    } else {
        LOG_WRN("Unknown button command");
        snprintf(uart_buffer, sizeof(uart_buffer), "ERROR: Unknown button command\r\n");
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
    }
}

/**
 * @brief Test sequence timer handler
 */
static void test_sequence_timer_handler(struct k_timer *timer)
{
    if (current_test_step < sizeof(test_step_descriptions) / sizeof(test_step_descriptions[0]) - 1) {
        LOG_INF("Test step %d: %s", current_test_step, test_step_descriptions[current_test_step]);
        snprintf(uart_buffer, sizeof(uart_buffer), "Test step %d: %s\r\n", 
                 current_test_step, test_step_descriptions[current_test_step]);
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
        
        switch (current_test_step) {
            case 0: /* Turn on all LEDs */
                for (int i = 0; i < GPIO_LED_COUNT; i++) {
                    if (gpio_control_led_is_ready(i)) {
                        gpio_control_led_on(i);
                    }
                }
                k_timer_start(&test_sequence_timer, K_MSEC(1000), K_NO_WAIT);
                break;
                
            case 1: /* Turn off all LEDs */
                for (int i = 0; i < GPIO_LED_COUNT; i++) {
                    if (gpio_control_led_is_ready(i)) {
                        gpio_control_led_off(i);
                    }
                }
                k_timer_start(&test_sequence_timer, K_MSEC(1000), K_NO_WAIT);
                break;
                
            case 2: /* Toggle LEDs sequentially */
                for (int i = 0; i < GPIO_LED_COUNT; i++) {
                    if (gpio_control_led_is_ready(i)) {
                        gpio_control_led_toggle(i);
                        k_sleep(K_MSEC(300));
                        gpio_control_led_toggle(i);
                    }
                }
                k_timer_start(&test_sequence_timer, K_MSEC(500), K_NO_WAIT);
                break;
                
            case 3: /* Blink LEDs at different rates */
                for (int i = 0; i < GPIO_LED_COUNT; i++) {
                    if (gpio_control_led_is_ready(i)) {
                        gpio_control_led_blink(i, 500 + i * 500);
                    }
                }
                k_timer_start(&test_sequence_timer, K_MSEC(3000), K_NO_WAIT);
                break;
                
            case 4: /* Simulate button press */
                gpio_test_simulate_button_press(500);
                k_timer_start(&test_sequence_timer, K_MSEC(1000), K_NO_WAIT);
                break;
        }
        
        current_test_step++;
    } else {
        /* Test sequence complete */
        LOG_INF("Test sequence complete");
        snprintf(uart_buffer, sizeof(uart_buffer), "Test sequence complete\r\n");
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
        
        /* Stop all LED blinking */
        for (int i = 0; i < GPIO_LED_COUNT; i++) {
            if (gpio_control_led_is_ready(i)) {
                gpio_control_led_blink_stop(i);
                gpio_control_led_off(i);
            }
        }
        
        test_running = false;
    }
}

int gpio_test_init(void)
{
    LOG_INF("Initializing GPIO test module");
    
    /* Initialize the button release work */
    k_work_init_delayable(&button_release_work, button_release_work_handler);
    
    /* Initialize the test sequence timer */
    k_timer_init(&test_sequence_timer, test_sequence_timer_handler, NULL);
    
    /* Reset test state */
    test_running = false;
    current_test_step = 0;
    
    LOG_INF("GPIO test module initialized");
    return 0;
}

int gpio_test_run_sequence(void)
{
    if (test_running) {
        LOG_WRN("Test sequence already running");
        return -1;
    }
    
    LOG_INF("Starting GPIO test sequence");
    snprintf(uart_buffer, sizeof(uart_buffer), "Starting GPIO test sequence\r\n");
    uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
    
    /* Print LED status */
    print_led_status();
    
    /* Reset test state */
    test_running = true;
    current_test_step = 0;
    
    /* Start the test sequence timer */
    k_timer_start(&test_sequence_timer, K_NO_WAIT, K_NO_WAIT);
    
    return 0;
}

int gpio_test_process_command(const char *cmd, size_t cmd_len)
{
    char cmd_copy[64];
    
    /* Check parameters */
    if (cmd == NULL || cmd_len == 0) {
        return -1;
    }
    
    /* Make a copy of the command and ensure null termination */
    if (cmd_len >= sizeof(cmd_copy)) {
        cmd_len = sizeof(cmd_copy) - 1;
    }
    memcpy(cmd_copy, cmd, cmd_len);
    cmd_copy[cmd_len] = '\0';
    
    /* Convert to lowercase for case-insensitive comparison */
    for (size_t i = 0; i < cmd_len; i++) {
        if (cmd_copy[i] >= 'A' && cmd_copy[i] <= 'Z') {
            cmd_copy[i] = cmd_copy[i] - 'A' + 'a';
        }
    }
    
    LOG_INF("Processing command: %s", cmd_copy);
    
    /* Handle command */
    if (strcmp(cmd_copy, "help") == 0) {
        gpio_test_print_help();
    } else if (strcmp(cmd_copy, "status") == 0) {
        print_led_status();
        snprintf(uart_buffer, sizeof(uart_buffer), "GPIO status printed to console log\r\n");
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
    } else if (strcmp(cmd_copy, "test") == 0) {
        gpio_test_run_sequence();
    } else if (strncmp(cmd_copy, "led", 3) == 0) {
        handle_led_command(cmd_copy);
    } else if (strncmp(cmd_copy, "button", 6) == 0) {
        handle_button_command(cmd_copy);
    } else {
        LOG_WRN("Unknown command: %s", cmd_copy);
        snprintf(uart_buffer, sizeof(uart_buffer), "ERROR: Unknown command. Type 'help' for available commands\r\n");
        uart_comm_send((uint8_t *)uart_buffer, strlen(uart_buffer), K_FOREVER);
        return -1;
    }
    
    return 0;
}

void gpio_test_print_help(void)
{
    const char *help_text =
        "\r\n===== GPIO Test Commands =====\r\n"
        "help                     - Show this help\r\n"
        "status                   - Show GPIO status\r\n"
        "test                     - Run GPIO test sequence\r\n"
        "led <index> on           - Turn on LED (index: 0=red, 1=green, 2=blue)\r\n"
        "led <index> off          - Turn off LED\r\n"
        "led <index> toggle       - Toggle LED\r\n"
        "led <index> blink <ms>   - Start LED blinking with period in ms\r\n"
        "led <index> stop         - Stop LED blinking\r\n"
        "button press <ms>        - Simulate button press with duration in ms\r\n"
        "==============================\r\n\r\n";
    
    LOG_INF("Printing help text");
    uart_comm_send((uint8_t *)help_text, strlen(help_text), K_FOREVER);
}

int gpio_test_simulate_button_press(uint32_t duration_ms)
{
    /* Only allow simulation if button is configured */
    if (!gpio_control_button_is_ready()) {
        LOG_ERR("Button is not ready for simulation");
        return -1;
    }
    
    /* Get access to the button GPIO spec */
    extern const struct gpio_dt_spec button_user;
    
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    /* Simulate button press by forcing pin value to active state */
    gpio_pin_set_dt(&button_user, 1);
    LOG_INF("Button press simulated");
    
    /* Schedule release after the specified duration */
    k_work_schedule(&button_release_work, K_MSEC(duration_ms));
    return 0;
#else
    LOG_ERR("Button node not defined in device tree");
    return -1;
#endif
}
