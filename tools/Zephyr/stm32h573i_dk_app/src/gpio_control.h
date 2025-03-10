/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GPIO_CONTROL_H
#define GPIO_CONTROL_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

/* Return codes */
#define GPIO_CTRL_SUCCESS         0
#define GPIO_CTRL_ERR_NOT_FOUND  -1
#define GPIO_CTRL_ERR_INIT       -2
#define GPIO_CTRL_ERR_PARAM      -3
#define GPIO_CTRL_ERR_STATE      -4

/* LED devicetree specifications */
#define LED_RED_NODE    DT_ALIAS(led0)  /* Red LED */
#define LED_GREEN_NODE  DT_ALIAS(led1)  /* Green LED */
#define LED_BLUE_NODE   DT_ALIAS(led2)  /* Blue LED */

/* Button devicetree specifications */
#define BUTTON_USER_NODE DT_ALIAS(sw0)  /* User button */

/* Check device tree nodes */
#if !DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
#warning "Red LED devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
#warning "Green LED devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(LED_BLUE_NODE, okay)
#warning "Blue LED devicetree alias is not defined"
#endif

#if !DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
#warning "User button devicetree alias is not defined"
#endif

/* Device tree specs for LEDs and button */
extern const struct gpio_dt_spec led_red;
extern const struct gpio_dt_spec led_green;
extern const struct gpio_dt_spec led_blue;
extern const struct gpio_dt_spec button_user;

/* LED indexes for API simplicity */
typedef enum {
    GPIO_LED_RED = 0,
    GPIO_LED_GREEN,
    GPIO_LED_BLUE,
    GPIO_LED_COUNT
} gpio_led_t;

/* Button callback function type */
typedef void (*gpio_button_callback_t)(const struct device *dev, 
                                      struct gpio_callback *cb, 
                                      uint32_t pins);

/**
 * @brief Initialize the GPIO control module
 * 
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_init(void);

/**
 * @brief Check if a specific LED is ready and configured
 * 
 * @param led LED index to check
 * @return true if LED is ready, false otherwise
 */
bool gpio_control_led_is_ready(gpio_led_t led);

/**
 * @brief Turn on an LED
 * 
 * @param led LED index
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_led_on(gpio_led_t led);

/**
 * @brief Turn off an LED
 * 
 * @param led LED index
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_led_off(gpio_led_t led);

/**
 * @brief Toggle an LED state
 * 
 * @param led LED index
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_led_toggle(gpio_led_t led);

/**
 * @brief Start blinking an LED with specified period
 * 
 * @param led LED index
 * @param period_ms Blink period in milliseconds
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_led_blink(gpio_led_t led, uint32_t period_ms);

/**
 * @brief Stop LED blinking
 * 
 * @param led LED index
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_led_blink_stop(gpio_led_t led);

/**
 * @brief Check if the user button is ready and configured
 * 
 * @return true if the button is ready, false otherwise
 */
bool gpio_control_button_is_ready(void);

/**
 * @brief Register callback for button press events
 * 
 * @param callback Function to call on button events
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_button_register_callback(gpio_button_callback_t callback);

/**
 * @brief Get current button state
 * 
 * @param state Pointer to store state (1=pressed, 0=released)
 * @return int GPIO_CTRL_SUCCESS on success, error code otherwise
 */
int gpio_control_button_get_state(int *state);

#endif /* GPIO_CONTROL_H */
