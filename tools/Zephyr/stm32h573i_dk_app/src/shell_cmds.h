/*
 * Copyright (c) 2025 CRUSTy-Core
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SHELL_CMDS_H_
#define SHELL_CMDS_H_

/**
 * @brief Initialize the shell commands for crypto operations
 * 
 * Registers all crypto-related shell commands with the Zephyr shell subsystem.
 * 
 * @return 0 on success, negative value on error
 */
int shell_cmds_init(void);

#endif /* SHELL_CMDS_H_ */
