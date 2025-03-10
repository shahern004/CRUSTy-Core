/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gpio_control.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(gpio_control, CONFIG_LOG_DEFAULT_LEVEL);

/* LED device tree specs */
#if DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
const struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
#endif

#if DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
#endif

#if DT_NODE_HAS_STATUS(LED_BLUE_NODE, okay)
const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);
#endif

/* Button device tree spec */
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
const struct gpio_dt_spec button_user = GPIO_DT_SPEC_GET(BUTTON_USER_NODE, gpios);
#endif

/* Array to make iterating over LEDs easier */
static const struct gpio_dt_spec *leds[GPIO_LED_COUNT] = {
#if DT_NODE_HAS_STATUS(LED_RED_NODE, okay)
    &led_red,
#else
    NULL,
#endif
#if DT_NODE_HAS_STATUS(LED_GREEN_NODE, okay)
    &led_green,
#else
    NULL,
#endif
#if DT_NODE_HAS_STATUS(LED_BLUE_NODE, okay)
    &led_blue,
#else
    NULL,
#endif
};

/* Button callback data */
static struct gpio_callback button_cb_data;
static gpio_button_callback_t button_callback_fn;

/* LED blinking control */
static struct k_timer led_blink_timers[GPIO_LED_COUNT];
static bool led_blink_states[GPIO_LED_COUNT];
static bool led_blinking[GPIO_LED_COUNT];

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
/* QEMU simulation state variables */
static bool qemu_led_states[GPIO_LED_COUNT] = {false, false, false};
static bool qemu_button_state = false;
static bool qemu_button_interrupt_enabled = false;
#endif

/* Private helper functions */
static void led_blink_handler(struct k_timer *timer);

/**
 * @brief LED blink timer handler - used for all LED blink timers
 */
static void led_blink_handler(struct k_timer *timer)
{
    /* Find which LED timer expired */
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        if (timer == &led_blink_timers[i] && led_blinking[i]) {
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
            /* Toggle the simulated LED in QEMU */
            qemu_led_states[i] = !qemu_led_states[i];
            led_blink_states[i] = qemu_led_states[i];
            LOG_DBG("QEMU: LED %d toggled to %d", i, qemu_led_states[i]);
#else
            /* Toggle the physical LED on STM32H573I-DK */
            if (leds[i] != NULL) {
                gpio_pin_toggle_dt(leds[i]);
                led_blink_states[i] = !led_blink_states[i];
            }
#endif
        }
    }
}

int gpio_control_init(void)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("Initializing GPIO control module (QEMU simulation)");
    
    /* Initialize simulated states */
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        qemu_led_states[i] = false;
        led_blinking[i] = false;
        led_blink_states[i] = false;
    }
    
    qemu_button_state = false;
    qemu_button_interrupt_enabled = false;
    button_callback_fn = NULL;
    
    /* Initialize LED blink timers */
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        k_timer_init(&led_blink_timers[i], led_blink_handler, NULL);
    }
    
    LOG_INF("QEMU GPIO simulation initialized successfully");
    return GPIO_CTRL_SUCCESS;
#else
    int ret;

    LOG_INF("Initializing GPIO control module (STM32H573I-DK hardware)");

    /* Initialize LEDs */
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        if (leds[i] != NULL) {
            if (!gpio_is_ready_dt(leds[i])) {
                LOG_ERR("LED %d device not ready", i);
                continue;
            }

            ret = gpio_pin_configure_dt(leds[i], GPIO_OUTPUT_INACTIVE);
            if (ret) {
                LOG_ERR("Failed to configure LED %d: %d", i, ret);
                continue;
            }

            LOG_INF("LED %d initialized successfully", i);
        }
    }

    /* Initialize button */
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    if (!gpio_is_ready_dt(&button_user)) {
        LOG_ERR("Button device not ready");
    } else {
        ret = gpio_pin_configure_dt(&button_user, GPIO_INPUT);
        if (ret) {
            LOG_ERR("Failed to configure button: %d", ret);
        } else {
            LOG_INF("Button initialized successfully");
        }
    }
#else
    LOG_WRN("Button not available in device tree");
#endif

    /* Initialize LED blink timers */
    for (int i = 0; i < GPIO_LED_COUNT; i++) {
        k_timer_init(&led_blink_timers[i], led_blink_handler, NULL);
        led_blinking[i] = false;
        led_blink_states[i] = false;
    }

    button_callback_fn = NULL;

    LOG_INF("GPIO control module initialized successfully");
    return GPIO_CTRL_SUCCESS;
#endif
}

bool gpio_control_led_is_ready(gpio_led_t led)
{
    if (led >= GPIO_LED_COUNT) {
        return false;
    }
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* LEDs are always "ready" in QEMU simulation */
    return true;
#else
    if (leds[led] == NULL) {
        return false;
    }
    
    return gpio_is_ready_dt(leds[led]);
#endif
}

int gpio_control_led_on(gpio_led_t led)
{
    if (led >= GPIO_LED_COUNT) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Set the simulated LED state in QEMU */
    qemu_led_states[led] = true;
    
    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }
    
    LOG_DBG("QEMU: LED %d turned ON", led);
    return GPIO_CTRL_SUCCESS;
#else
    if (leds[led] == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

    if (!gpio_is_ready_dt(leds[led])) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }

    return gpio_pin_set_dt(leds[led], 1);
#endif
}

int gpio_control_led_off(gpio_led_t led)
{
    if (led >= GPIO_LED_COUNT) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Set the simulated LED state in QEMU */
    qemu_led_states[led] = false;
    
    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }
    
    LOG_DBG("QEMU: LED %d turned OFF", led);
    return GPIO_CTRL_SUCCESS;
#else
    if (leds[led] == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

    if (!gpio_is_ready_dt(leds[led])) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }

    return gpio_pin_set_dt(leds[led], 0);
#endif
}

int gpio_control_led_toggle(gpio_led_t led)
{
    if (led >= GPIO_LED_COUNT) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Toggle the simulated LED state in QEMU */
    qemu_led_states[led] = !qemu_led_states[led];
    
    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }
    
    LOG_DBG("QEMU: LED %d toggled to %d", led, qemu_led_states[led]);
    return GPIO_CTRL_SUCCESS;
#else
    if (leds[led] == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

    if (!gpio_is_ready_dt(leds[led])) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Stop blinking if active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }

    return gpio_pin_toggle_dt(leds[led]);
#endif
}

int gpio_control_led_blink(gpio_led_t led, uint32_t period_ms)
{
    if (led >= GPIO_LED_COUNT || period_ms == 0) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Stop current blinking if already active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }

    /* Start the blink timer for QEMU simulation */
    led_blinking[led] = true;
    k_timer_start(&led_blink_timers[led], K_MSEC(period_ms / 2), K_MSEC(period_ms / 2));
    
    LOG_DBG("QEMU: LED %d blinking started with period %d ms", led, period_ms);
    return GPIO_CTRL_SUCCESS;
#else
    if (leds[led] == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

    if (!gpio_is_ready_dt(leds[led])) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Stop current blinking if already active */
    if (led_blinking[led]) {
        gpio_control_led_blink_stop(led);
    }

    /* Start the blink timer */
    led_blinking[led] = true;
    k_timer_start(&led_blink_timers[led], K_MSEC(period_ms / 2), K_MSEC(period_ms / 2));
    
    return GPIO_CTRL_SUCCESS;
#endif
}

int gpio_control_led_blink_stop(gpio_led_t led)
{
    if (led >= GPIO_LED_COUNT) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    if (!led_blinking[led]) {
        return GPIO_CTRL_SUCCESS;  /* Already stopped */
    }

    /* Stop the blink timer */
    k_timer_stop(&led_blink_timers[led]);
    led_blinking[led] = false;
    
    LOG_DBG("QEMU: LED %d blinking stopped", led);
    return GPIO_CTRL_SUCCESS;
#else
    if (leds[led] == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

    if (!gpio_is_ready_dt(leds[led])) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    if (!led_blinking[led]) {
        return GPIO_CTRL_SUCCESS;  /* Already stopped */
    }

    /* Stop the blink timer */
    k_timer_stop(&led_blink_timers[led]);
    led_blinking[led] = false;
    
    return GPIO_CTRL_SUCCESS;
#endif
}

bool gpio_control_button_is_ready(void)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Button is always "ready" in QEMU simulation */
    return true;
#else
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    return gpio_is_ready_dt(&button_user);
#else
    return false;
#endif
#endif
}

int gpio_control_button_register_callback(gpio_button_callback_t callback)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Store the callback for QEMU simulation */
    button_callback_fn = callback;
    qemu_button_interrupt_enabled = (callback != NULL);
    
    LOG_DBG("QEMU: Button callback %s", 
            qemu_button_interrupt_enabled ? "registered" : "unregistered");
    return GPIO_CTRL_SUCCESS;
#else
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    int ret;
    
    if (!gpio_is_ready_dt(&button_user)) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Store the callback */
    button_callback_fn = callback;

    if (callback != NULL) {
        /* Configure interrupts if we have a callback */
        ret = gpio_pin_interrupt_configure_dt(&button_user, GPIO_INT_EDGE_BOTH);
        if (ret) {
            LOG_ERR("Failed to configure button interrupt: %d", ret);
            return GPIO_CTRL_ERR_INIT;
        }

        /* Initialize and add the callback */
        gpio_init_callback(&button_cb_data, callback, BIT(button_user.pin));
        return gpio_add_callback(button_user.port, &button_cb_data);
    } else {
        /* Remove interrupt config and callback when NULL */
        gpio_pin_interrupt_configure_dt(&button_user, GPIO_INT_DISABLE);
        return gpio_remove_callback(button_user.port, &button_cb_data);
    }
#else
    return GPIO_CTRL_ERR_NOT_FOUND;
#endif
#endif
}

int gpio_control_button_get_state(int *state)
{
    if (state == NULL) {
        return GPIO_CTRL_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Return the simulated button state in QEMU */
    *state = qemu_button_state ? 1 : 0;
    
    LOG_DBG("QEMU: Button state read: %d", *state);
    return GPIO_CTRL_SUCCESS;
#else
#if DT_NODE_HAS_STATUS(BUTTON_USER_NODE, okay)
    if (!gpio_is_ready_dt(&button_user)) {
        return GPIO_CTRL_ERR_NOT_FOUND;
    }

    /* Read the button state */
    *state = gpio_pin_get_dt(&button_user);
    return GPIO_CTRL_SUCCESS;
#else
    return GPIO_CTRL_ERR_NOT_FOUND;
#endif
#endif
}

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
/* QEMU-specific function to simulate a button press (for testing) */
int gpio_control_qemu_simulate_button_press(bool pressed)
{
    qemu_button_state = pressed;
    
    LOG_INF("QEMU: Button simulated %s", pressed ? "PRESSED" : "RELEASED");
    
    /* If a callback is registered and interrupts are enabled, trigger it */
    if (button_callback_fn != NULL && qemu_button_interrupt_enabled) {
        /* Simulate a callback with dummy values - the callback function
         * will typically call gpio_control_button_get_state() to get the state */
        button_callback_fn(NULL, NULL, 0);
        LOG_DBG("QEMU: Button callback triggered");
    }
    
    return GPIO_CTRL_SUCCESS;
}
#endif
