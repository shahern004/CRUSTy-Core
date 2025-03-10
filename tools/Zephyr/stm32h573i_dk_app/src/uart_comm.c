/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "uart_comm.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>

#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
/* Hardware-specific includes */
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#endif

LOG_MODULE_REGISTER(uart_comm, CONFIG_LOG_DEFAULT_LEVEL);

/* Private variables */
static uart_rx_callback_t user_rx_callback;
static uint8_t rx_buf[UART_COMM_RX_BUFFER_SIZE];
static uint8_t rx_secondary_buf[UART_COMM_RX_BUFFER_SIZE];
static size_t rx_buf_pos;
static bool uart_initialized = false;

/* For thread-safe operation */
static K_MUTEX_DEFINE(uart_mutex);

#ifndef CONFIG_BOARD_QEMU_CORTEX_M3
/* Hardware-specific variables */
static const struct device *uart_dev;

/* UART interrupt callback to handle received data */
static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t c;

    if (!uart_irq_update(dev)) {
        return;
    }

    if (uart_irq_rx_ready(dev)) {
        /* Read all available data */
        while (uart_fifo_read(dev, &c, 1) == 1) {
            if (rx_buf_pos < UART_COMM_RX_BUFFER_SIZE - 1) {
                rx_buf[rx_buf_pos++] = c;
                
                /* Check for line ending or buffer full */
                if (c == '\n' || rx_buf_pos >= UART_COMM_RX_BUFFER_SIZE - 1) {
                    /* Null terminate the string */
                    rx_buf[rx_buf_pos] = '\0';
                    
                    /* Process the received data */
                    if (user_rx_callback != NULL) {
                        /* Copy data to secondary buffer for callback processing */
                        k_mutex_lock(&uart_mutex, K_FOREVER);
                        memcpy(rx_secondary_buf, rx_buf, rx_buf_pos + 1);
                        size_t len = rx_buf_pos;
                        rx_buf_pos = 0;
                        k_mutex_unlock(&uart_mutex);
                        
                        /* Call user callback with received data */
                        user_rx_callback(rx_secondary_buf, len);
                    } else {
                        rx_buf_pos = 0;
                    }
                }
            } else {
                /* Buffer overflow - reset position */
                rx_buf_pos = 0;
                LOG_WRN("UART RX buffer overflow");
            }
        }
    }

    if (uart_irq_tx_ready(dev)) {
        /* TX handling if needed */
    }
}
#else
/* QEMU simulation variables */
static uint8_t qemu_tx_buf[UART_COMM_TX_BUFFER_SIZE];
static size_t qemu_tx_buf_pos = 0;
static bool qemu_data_available = false;
static struct k_work_delayable qemu_rx_sim_work;

/* Simulated work handler to process "received" data for QEMU demo */
static void qemu_rx_sim_handler(struct k_work *work)
{
    static const char *sim_responses[] = {
        "QEMU simulated response 1\n",
        "QEMU simulated response 2\n",
        "QEMU device ready\n",
        "QEMU status: OK\n"
    };
    static int resp_idx = 0;
    
    /* Only simulate reception if callback is registered */
    if (user_rx_callback != NULL) {
        const char *resp = sim_responses[resp_idx];
        size_t len = strlen(resp);
        
        /* Copy simulated data to the RX buffer */
        k_mutex_lock(&uart_mutex, K_FOREVER);
        memcpy(rx_buf, resp, len);
        rx_buf[len] = '\0';
        rx_buf_pos = len;
        
        LOG_DBG("QEMU: Simulating received data: %s", resp);
        
        /* Copy data for callback */
        memcpy(rx_secondary_buf, rx_buf, len + 1);
        rx_buf_pos = 0;
        k_mutex_unlock(&uart_mutex);
        
        /* Call user callback with "received" data */
        user_rx_callback(rx_secondary_buf, len);
        
        /* Cycle through response options */
        resp_idx = (resp_idx + 1) % (sizeof(sim_responses) / sizeof(sim_responses[0]));
        
        /* Schedule next simulated response for demo purposes */
        k_work_schedule(&qemu_rx_sim_work, K_MSEC(5000));
    }
}
#endif

int uart_comm_init(const char *device_name, uint32_t baud_rate, uart_rx_callback_t rx_callback)
{
    if (device_name == NULL) {
        LOG_ERR("UART device name is NULL");
        return UART_COMM_ERR_PARAM;
    }

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    LOG_INF("Initializing UART communication (QEMU simulation)");
    
    /* Store the callback function */
    user_rx_callback = rx_callback;
    
    /* Initialize the receive buffer */
    rx_buf_pos = 0;
    qemu_tx_buf_pos = 0;
    
    /* Initialize the work item for simulating RX in QEMU */
    k_work_init_delayable(&qemu_rx_sim_work, qemu_rx_sim_handler);
    
    /* Start the simulation if a callback was provided */
    if (rx_callback != NULL) {
        k_work_schedule(&qemu_rx_sim_work, K_MSEC(3000));
        LOG_DBG("QEMU: Scheduled simulated UART reception");
    }
    
    uart_initialized = true;
    LOG_INF("QEMU UART simulation initialized at %d baud", baud_rate);
    return UART_COMM_SUCCESS;
#else
    /* Get the UART device */
    uart_dev = device_get_binding(device_name);
    if (uart_dev == NULL) {
        LOG_ERR("Failed to get UART device: %s", device_name);
        return UART_COMM_ERR_NOT_FOUND;
    }

    /* Store the callback function */
    user_rx_callback = rx_callback;

    /* Initialize the UART with the specified baud rate */
    struct uart_config cfg;
    int ret = uart_config_get(uart_dev, &cfg);
    if (ret != 0) {
        LOG_ERR("Failed to get UART config: %d", ret);
        return UART_COMM_ERR_INIT;
    }

    cfg.baudrate = baud_rate;
    cfg.parity = UART_CFG_PARITY_NONE;
    cfg.stop_bits = UART_CFG_STOP_BITS_1;
    cfg.data_bits = UART_CFG_DATA_BITS_8;
    cfg.flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

    ret = uart_configure(uart_dev, &cfg);
    if (ret != 0) {
        LOG_ERR("Failed to configure UART: %d", ret);
        return UART_COMM_ERR_INIT;
    }

    /* Initialize the receive buffer */
    rx_buf_pos = 0;

    /* Set up the interrupt callback if a user callback was provided */
    if (rx_callback != NULL) {
        uart_irq_callback_set(uart_dev, uart_cb);
        uart_irq_rx_enable(uart_dev);
    }

    uart_initialized = true;
    LOG_INF("UART communication initialized on %s at %d baud", device_name, baud_rate);
    return UART_COMM_SUCCESS;
#endif
}

int uart_comm_send(const uint8_t *data, size_t len, k_timeout_t timeout)
{
    if (!uart_initialized) {
        LOG_ERR("UART not initialized");
        return UART_COMM_ERR_STATE;
    }

    if (data == NULL || len == 0) {
        LOG_ERR("Invalid UART send parameters");
        return UART_COMM_ERR_PARAM;
    }

    k_mutex_lock(&uart_mutex, K_FOREVER);
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* In QEMU simulation, just store the data in our buffer */
    size_t copy_len = MIN(len, UART_COMM_TX_BUFFER_SIZE - qemu_tx_buf_pos);
    
    if (copy_len < len) {
        LOG_WRN("QEMU UART TX buffer overflow, truncating");
    }
    
    /* Copy data to our simulated TX buffer */
    memcpy(&qemu_tx_buf[qemu_tx_buf_pos], data, copy_len);
    qemu_tx_buf_pos += copy_len;
    
    /* Null-terminate the buffer for debug output */
    size_t null_pos = MIN(qemu_tx_buf_pos, UART_COMM_TX_BUFFER_SIZE - 1);
    qemu_tx_buf[null_pos] = '\0';
    
    /* In simulation, print what would have been sent */
    LOG_DBG("QEMU UART TX (%d bytes): %s", (int)len, data);
    
    k_mutex_unlock(&uart_mutex);
    return UART_COMM_SUCCESS;
#else
    /* Send data using Zephyr's UART API */
    for (size_t i = 0; i < len; i++) {
        /* Poll until UART TX buffer is not full or timeout */
        int64_t start_time = k_uptime_get();
        bool tx_timeout = false;
        
        while (uart_fifo_fill(uart_dev, &data[i], 1) != 1) {
            /* Check for timeout if not waiting forever */
            if (timeout.ticks != K_FOREVER.ticks && timeout.ticks != K_NO_WAIT.ticks) {
                if (k_uptime_get() - start_time > timeout.ticks) {
                    tx_timeout = true;
                    break;
                }
            }
            
            /* For non-blocking mode, return immediately */
            if (timeout.ticks == K_NO_WAIT.ticks) {
                tx_timeout = true;
                break;
            }
            
            k_yield();
        }
        
        if (tx_timeout) {
            k_mutex_unlock(&uart_mutex);
            LOG_ERR("UART TX timeout");
            return UART_COMM_ERR_TIMEOUT;
        }
    }
    
    k_mutex_unlock(&uart_mutex);
    return UART_COMM_SUCCESS;
#endif
}

int uart_comm_receive(uint8_t *data, size_t max_len, k_timeout_t timeout)
{
    if (!uart_initialized) {
        LOG_ERR("UART not initialized");
        return UART_COMM_ERR_STATE;
    }

    if (data == NULL || max_len == 0) {
        LOG_ERR("Invalid UART receive parameters");
        return UART_COMM_ERR_PARAM;
    }

    /* In interrupt mode, this function is less useful */
    if (user_rx_callback != NULL) {
        LOG_WRN("UART is in interrupt mode, polling receive may interfere with callback");
    }

    k_mutex_lock(&uart_mutex, K_FOREVER);
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* For QEMU simulation, we'll create simulated data if requested */
    static int qemu_read_counter = 0;
    size_t recv_count = 0;

    /* Simulate some delay */
    k_sleep(K_MSEC(10));
    
    /* Every few reads, generate a predefined response */
    if (++qemu_read_counter % 3 == 0) {
        const char *resp = "QEMU polled data\n";
        size_t resp_len = strlen(resp);
        size_t copy_len = MIN(resp_len, max_len - 1);
        
        /* Copy simulated response */
        memcpy(data, resp, copy_len);
        recv_count = copy_len;
        
        /* Null-terminate for string safety */
        if (recv_count < max_len) {
            data[recv_count] = '\0';
        }
        
        LOG_DBG("QEMU: Simulated polled reception (%d bytes)", (int)recv_count);
    } else {
        /* Simulate no data available */
        recv_count = 0;
        LOG_DBG("QEMU: No data available for polling");
    }
    
    k_mutex_unlock(&uart_mutex);
    return (recv_count > 0) ? recv_count : UART_COMM_ERR_TIMEOUT;
#else
    size_t recv_count = 0;
    int64_t start_time = k_uptime_get();
    
    while (recv_count < max_len) {
        uint8_t c;
        int ret = uart_poll_in(uart_dev, &c);
        
        if (ret == 0) {
            /* Data received */
            data[recv_count++] = c;
            
            /* Return if we received a newline or reached max length */
            if (c == '\n' || recv_count >= max_len) {
                break;
            }
        } else {
            /* No data available, check timeout */
            if (timeout.ticks != K_FOREVER.ticks) {
                if (timeout.ticks == K_NO_WAIT.ticks || 
                    (k_uptime_get() - start_time > timeout.ticks)) {
                    break;
                }
            }
            
            /* Yield to other threads */
            k_yield();
        }
    }
    
    k_mutex_unlock(&uart_mutex);
    
    /* Return number of bytes received or error code */
    return (recv_count > 0) ? recv_count : UART_COMM_ERR_TIMEOUT;
#endif
}

int uart_comm_flush_rx(void)
{
    if (!uart_initialized) {
        LOG_ERR("UART not initialized");
        return UART_COMM_ERR_STATE;
    }

    k_mutex_lock(&uart_mutex, K_FOREVER);
    
    /* Reset the receive buffer position */
    rx_buf_pos = 0;
    
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* Clear simulated data */
    LOG_DBG("QEMU: Flushing RX buffer");
#else
    /* Flush any pending data from the UART FIFO */
    uint8_t dummy;
    while (uart_poll_in(uart_dev, &dummy) == 0) {
        /* Discard received character */
    }
#endif
    
    k_mutex_unlock(&uart_mutex);
    return UART_COMM_SUCCESS;
}

bool uart_comm_is_ready(void)
{
#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
    /* UART is always "ready" in QEMU simulation */
    return uart_initialized;
#else
    return uart_initialized && device_is_ready(uart_dev);
#endif
}

#ifdef CONFIG_BOARD_QEMU_CORTEX_M3
/* QEMU-specific function to inject data for testing */
int uart_comm_qemu_inject_data(const uint8_t *data, size_t len)
{
    if (!uart_initialized) {
        LOG_ERR("UART not initialized");
        return UART_COMM_ERR_STATE;
    }

    if (data == NULL || len == 0) {
        LOG_ERR("Invalid data parameters");
        return UART_COMM_ERR_PARAM;
    }

    /* Only inject if we have a callback registered */
    if (user_rx_callback == NULL) {
        LOG_WRN("QEMU: No RX callback registered, can't inject data");
        return UART_COMM_ERR_STATE;
    }

    k_mutex_lock(&uart_mutex, K_FOREVER);
    
    /* Copy injected data to the RX buffer */
    size_t copy_len = MIN(len, UART_COMM_RX_BUFFER_SIZE - 1);
    
    if (copy_len < len) {
        LOG_WRN("QEMU UART RX buffer overflow, truncating injected data");
    }
    
    memcpy(rx_buf, data, copy_len);
    rx_buf[copy_len] = '\0';
    
    /* Copy to secondary buffer for callback */
    memcpy(rx_secondary_buf, rx_buf, copy_len + 1);
    
    LOG_DBG("QEMU: Injecting %d bytes of data", (int)copy_len);
    
    k_mutex_unlock(&uart_mutex);
    
    /* Call the user callback with the injected data */
    user_rx_callback(rx_secondary_buf, copy_len);
    
    return UART_COMM_SUCCESS;
}
#endif
