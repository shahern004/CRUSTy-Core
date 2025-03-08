/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

/* LED device */
#define LED_DEV_NAME "GPIOI"
#define LED_PIN 8  /* Green LED on STM32H573I-DK */
#define LED_FLAGS 0

/* UART device */
#define UART_DEV_NAME "USART1"

/* For QEMU compatibility */
#ifdef QEMU
#define LED_DEV_NAME_QEMU "GPIO_0"
#define LED_PIN_QEMU 0
#define UART_DEV_NAME_QEMU "UART_0"
#endif

/* Buffer for UART data */
static uint8_t uart_rx_buf[64];
static uint8_t uart_tx_buf[64];
static int uart_rx_index = 0;

/* UART callback function */
static void uart_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(dev)) {
		return;
	}

	if (uart_irq_rx_ready(dev)) {
		/* Read character from UART */
		while (uart_fifo_read(dev, &c, 1) == 1) {
			/* Echo character back */
			uart_tx_buf[0] = c;
			uart_fifo_fill(dev, uart_tx_buf, 1);

			/* Store in buffer */
			if (uart_rx_index < sizeof(uart_rx_buf) - 1) {
				uart_rx_buf[uart_rx_index++] = c;
			}

			/* Process complete line */
			if (c == '\r' || c == '\n') {
				uart_rx_buf[uart_rx_index] = '\0';
				LOG_INF("Received: %s", uart_rx_buf);
				uart_rx_index = 0;
			}
		}
	}
}

void main(void)
{
	const struct device *led_dev;
	const struct device *uart_dev;
	bool led_state = false;
	int ret;
	const char *uart_name = UART_DEV_NAME;
	const char *led_name = LED_DEV_NAME;
	int led_pin = LED_PIN;

	LOG_INF("STM32H573I-DK QEMU Test Application");
	LOG_INF("Board: %s", CONFIG_BOARD);

#ifdef QEMU
	LOG_INF("Running in QEMU emulation mode");
	uart_name = UART_DEV_NAME_QEMU;
	led_name = LED_DEV_NAME_QEMU;
	led_pin = LED_PIN_QEMU;
#else
	LOG_INF("Running on real hardware");
#endif

	/* Initialize UART */
	uart_dev = device_get_binding(uart_name);
	if (!uart_dev) {
		LOG_ERR("UART device %s not found", uart_name);
	} else {
		LOG_INF("UART device %s initialized", uart_name);
		
		/* Configure UART */
		uart_irq_callback_set(uart_dev, uart_cb);
		uart_irq_rx_enable(uart_dev);

		/* Send welcome message */
		const char *msg = "STM32H573I-DK UART Echo Server\r\n";
		for (int i = 0; i < strlen(msg); i++) {
			uart_poll_out(uart_dev, msg[i]);
		}
	}

	/* Initialize LED */
	led_dev = device_get_binding(led_name);
	if (led_dev == NULL) {
		LOG_ERR("Failed to get LED device %s", led_name);
		LOG_INF("Using simulated LED");
		
		while (1) {
			led_state = !led_state;
			LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
			k_sleep(K_MSEC(1000));
		}
		return;
	}

	ret = gpio_pin_configure(led_dev, led_pin, GPIO_OUTPUT_ACTIVE | LED_FLAGS);
	if (ret < 0) {
		LOG_ERR("Failed to configure GPIO pin");
		return;
	}

	LOG_INF("LED device configured successfully");

	while (1) {
		led_state = !led_state;
		gpio_pin_set(led_dev, led_pin, led_state);
		LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
		k_sleep(K_MSEC(1000));
	}
}
