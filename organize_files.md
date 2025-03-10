# Files Organization Reference

## Primary Files for STM32/Zephyr builds

Files required in current pipeline (from tools/Zephyr/stm32h573i_dk_app/CMakeLists.txt):

- src/main.c
- src/gpio_control.c (and .h)
- src/uart_comm.c (and .h)
- src/uart_demo.c (and .h)
- src/crypto_ops.c (and .h)
- src/crypto_demo.c (and .h)
- src/gpio_test.c (and .h)
- src/mem_protect.c (and .h)
- src/mem_protect_demo.c (and .h)

## Primary Files for C++ application

Files required in current pipeline (from CMakeLists.txt):

- src/cpp/core/encryptor.cpp (and .h)
- src/cpp/core/file_operations.cpp (and .h)
- src/cpp/core/audit_log.h
- src/cpp/core/path_utils.h
- src/cpp/core/secure_utils.h
- src/cpp/main.cpp
- src/cpp/resources.qrc (if Qt UI is enabled)
- src/cpp/ui/\* (if Qt UI is enabled)

## Builds and Test Directories To Keep

- tools/QEMU Reference (essential reference materials)
- tools/Zephyr/stm32h573i_dk_app (main STM32 application)

## Added to gitignore (unused or build artifacts)

- tools/Zephyr/build\_\* (all build directories)
- tools/Zephyr/zephyr/ (framework code that should be pulled from upstream)
- tools/Zephyr/modules/ (external modules that should be pulled when needed)
- tools/Zephyr/bootloader/ (not currently used)
- tools/Zephyr/HAL/ (not currently used)
- tools/Zephyr/Modules/ (not currently used)

This organization preserves the current directory structure while identifying the essential files needed for both the STM32/QEMU builds and the C++ application.
