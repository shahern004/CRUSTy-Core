# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Important: Don't set BOARD_ROOT to avoid using custom board definitions
# This helps ensure we use the standard Zephyr board definitions only

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(stm32h573i_dk_app)

# Platform-specific defines for conditional compilation
if(CONFIG_BOARD_QEMU_CORTEX_M3)
    message(STATUS "Building for QEMU Cortex-M3 simulation")
    target_compile_definitions(app PRIVATE 
        -DPLATFORM_QEMU=1
        -DPLATFORM_STM32H573I_DK=0
        -DHW_AES_AVAILABLE=0
        -DHW_RNG_AVAILABLE=0
        -DHW_SHA_AVAILABLE=0
        -DHW_PKA_AVAILABLE=0
        -DUSE_SW_CRYPTO=1
    )
elseif(CONFIG_BOARD_STM32H573I_DK)
    message(STATUS "Building for STM32H573I-DK hardware")
    target_compile_definitions(app PRIVATE 
        -DPLATFORM_QEMU=0
        -DPLATFORM_STM32H573I_DK=1
        -DHW_AES_AVAILABLE=1
        -DHW_RNG_AVAILABLE=1
        -DHW_SHA_AVAILABLE=0
        -DHW_PKA_AVAILABLE=0
        -DUSE_SW_CRYPTO_FALLBACK=1
    )
else()
    message(WARNING "Unknown board target - using QEMU simulation defaults")
    target_compile_definitions(app PRIVATE 
        -DPLATFORM_QEMU=1
        -DPLATFORM_STM32H573I_DK=0
        -DHW_AES_AVAILABLE=0
        -DHW_RNG_AVAILABLE=0
        -DHW_SHA_AVAILABLE=0
        -DHW_PKA_AVAILABLE=0
        -DUSE_SW_CRYPTO=1
    )
endif()

# Source files
target_sources(app PRIVATE 
    src/main.c
    src/gpio_control.c
    src/uart_comm.c
    src/uart_demo.c
    src/crypto_ops.c
    src/crypto_demo.c
    src/gpio_test.c
    src/mem_protect.c
    src/mem_protect_demo.c
)

# Include directories
target_include_directories(app PRIVATE src)

# Print build information
message(STATUS "Board: ${BOARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
