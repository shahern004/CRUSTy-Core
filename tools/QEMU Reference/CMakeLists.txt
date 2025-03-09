# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Set Zephyr module paths
set(ZEPHYR_MODULES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/zephyr)
list(APPEND ZEPHYR_MODULES ${ZEPHYR_MODULES_DIR}/HAL/stm32)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(stm32h573i_dk_app)

# Define QEMU flag for conditional compilation
if(DEFINED QEMU)
    add_compile_definitions(QEMU=1)
endif()

# Include STM32 HAL headers
include_directories(${ZEPHYR_MODULES_DIR}/HAL/stm32/stm32cube/stm32h5xx/drivers/include)

# Include source files
target_sources(app PRIVATE
    main.c
)

# Include header directories
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add compile options for better error detection
target_compile_options(app PRIVATE
    -Wall
    -Wextra
    -Werror=implicit-function-declaration
)

# Configure logging
add_compile_definitions(
    CONFIG_LOG=1
    CONFIG_LOG_DEFAULT_LEVEL=3  # INFO level
)
