/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UART_COMM_H
#define UART_COMM_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

/* Return codes */
#define UART_COMM_SUCCESS         0
#define UART_COMM_ERR_NOT_FOUND  -1
#define UART_COMM_ERR_INIT       -2
#define UART_COMM_ERR_PARAM      -3
#define UART_COMM_ERR_STATE      -4
#define UART_COMM_ERR_TIMEOUT    -5

/* Buffer size for UART communication */
#define UART_COMM_RX_BUFFER_SIZE 256
#define UART_COMM_TX_BUFFER_SIZE 256

/* UART callback types */
typedef void (*uart_rx_callback_t)(const uint8_t *data, size_t len);

/**
 * @brief Initialize UART communication
 *
 * This function initializes the UART device with the specified configuration.
 *
 * @param device_name Name of the UART device (e.g., "USART1")
 * @param baud_rate Baud rate (e.g., 115200)
 * @param rx_callback Callback function for received data (can be NULL)
 *
 * @return UART_COMM_SUCCESS on success, error code on failure
 */
int uart_comm_init(const char *device_name, uint32_t baud_rate, uart_rx_callback_t rx_callback);

/**
 * @brief Send data over UART
 *
 * This function sends data over the initialized UART device.
 *
 * @param data Pointer to the data to send
 * @param len Length of the data in bytes
 * @param timeout_ms Maximum time to wait for transmission to complete in milliseconds
 *        (0 for non-blocking, K_FOREVER for blocking until complete)
 *
 * @return UART_COMM_SUCCESS on success, error code on failure
 */
int uart_comm_send(const uint8_t *data, size_t len, k_timeout_t timeout);

/**
 * @brief Receive data from UART
 *
 * This function receives data from the UART device into the provided buffer.
 * Note: This is for polling mode. For interrupt-driven reception, use the callback.
 *
 * @param data Pointer to the buffer to store received data
 * @param max_len Maximum length of data to receive
 * @param timeout_ms Maximum time to wait for reception in milliseconds
 *        (0 for non-blocking, K_FOREVER for blocking until data received)
 *
 * @return Number of bytes received on success, error code on failure
 */
int uart_comm_receive(uint8_t *data, size_t max_len, k_timeout_t timeout);

/**
 * @brief Clear UART receive buffer
 *
 * This function clears any pending data in the receive buffer.
 *
 * @return UART_COMM_SUCCESS on success, error code on failure
 */
int uart_comm_flush_rx(void);

/**
 * @brief Check if UART device is ready
 *
 * This function checks if the UART device is initialized and ready for communication.
 *
 * @return true if UART is ready, false otherwise
 */
bool uart_comm_is_ready(void);

#endif /* UART_COMM_H */
