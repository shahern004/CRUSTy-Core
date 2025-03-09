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

/* Include STM32 HAL headers - conditionally included to avoid QEMU build issues */
#ifndef QEMU
/* These includes are only needed for real hardware */
#include "../../tools/zephyr/HAL/stm32/stm32cube/stm32h5xx/drivers/include/stm32h5xx_hal.h"
#include "../../tools/zephyr/HAL/stm32/stm32cube/stm32h5xx/drivers/include/stm32h5xx_hal_gpio.h"
#include "../../tools/zephyr/HAL/stm32/stm32cube/stm32h5xx/drivers/include/stm32h5xx_hal_uart.h"
#include "../../tools/zephyr/HAL/stm32/stm32cube/stm32h5xx/drivers/include/stm32h5xx_hal_rcc.h"
#endif

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

/* Test message */
#define TEST_MESSAGE "Zephyr STM32 HAL Test: Hello, World!\r\n"
#define HAL_TEST_MESSAGE "Direct STM32 HAL Access Test\r\n"

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

/* SystemClock_Config function for STM32H573I-DK */
#ifndef QEMU
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Configure the main internal regulator output voltage */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  /* Wait for PWR_FLAG_VOSRDY */
  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Configure HSE as PLL source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Configure system clock */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}
#endif

/* STM32 HAL initialization function */
static void stm32_hal_init(void)
{
#ifndef QEMU
	/* Initialize the HAL Library */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	LOG_INF("STM32 HAL initialized");
#else
	LOG_INF("STM32 HAL initialization skipped in QEMU mode");
#endif
}

/* Example of direct HAL GPIO access */
static void hal_gpio_example(void)
{
#ifndef QEMU
	/* This is just an example and won't be executed in the main flow */
	/* It demonstrates how to access GPIO directly using the HAL */
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/* Enable the GPIO clock */
	__HAL_RCC_GPIOI_CLK_ENABLE();
	
	/* Configure the GPIO pin */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	
	/* Toggle the pin */
	HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_8);
	
	LOG_INF("Direct HAL GPIO access example");
#endif
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

	LOG_INF("STM32H573I-DK Test Application with Zephyr STM32 HAL");
	LOG_INF("Board: %s", CONFIG_BOARD);
	
	/* Initialize STM32 HAL */
	stm32_hal_init();

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
		for (int i = 0; i < strlen(TEST_MESSAGE); i++) {
			uart_poll_out(uart_dev, TEST_MESSAGE[i]);
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
	LOG_INF("LED will toggle every second");
	LOG_INF("UART echo is active");
	
	/* Send HAL test message */
	if (uart_dev) {
		for (int i = 0; i < strlen(HAL_TEST_MESSAGE); i++) {
			uart_poll_out(uart_dev, HAL_TEST_MESSAGE[i]);
		}
	}

	while (1) {
		led_state = !led_state;
		gpio_pin_set(led_dev, led_pin, led_state);
		LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
		k_sleep(K_MSEC(1000));
	}
}
