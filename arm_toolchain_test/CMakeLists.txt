cmake_minimum_required(VERSION 3.16)

# Project name and language
project(arm_toolchain_test C CXX ASM)

# Target processor definitions
add_definitions(-DSTM32H573xx -DUSE_HAL_DRIVER)

# Source files
set(SOURCES
    main.cpp
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create executable using the function from the toolchain file
add_stm32_executable(${PROJECT_NAME} ${SOURCES})
