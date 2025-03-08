# STM32H573I-DK Technical Reference

This document provides technical specifications and reference information for the STM32H573I-DK Discovery board used in the CRUSTy-Core project.

## Board Overview

The STM32H573I-DK Discovery kit is designed as a complete demonstration and development platform for STMicroelectronics Arm® Cortex®-M33 core-based STM32H573IIK3Q microcontroller with TrustZone®.

### Key Features

- **STM32H573IIK3Q** microcontroller featuring 2 Mbytes of Flash memory and 640 Kbytes of SRAM in 176-pin BGA package
- **1.54-inch 240x240 pixels TFT-LCD** with LED backlight and touch panel
- **USB Type-C™** Host and device with USB power-delivery controller
- **SAI Audio DAC** stereo with one audio jacks for input/output
- **ST MEMS digital microphone** with PDM interface
- **Octo-SPI interface** connected to 512Mbit Octo-SPI NORFlash memory device (MX25LM51245GXDI00 from MACRONIX)
- **10/100-Mbit Ethernet**
- **microSD™** card slot
- **Wi‑Fi® add-on board** support

### Board Connectors

- **STMod+ expansion connector** with fan-out expansion board for Wi‑Fi®, Grove and mikroBUS™ compatible connectors
- **Pmod™ expansion connector**
- **Audio MEMS daughterboard** expansion connector
- **ARDUINO® Uno V3 expansion connector**

### Power Supply Options

- ST-LINK
- USB VBUS
- External sources

### Debug Support

- On-board STLINK-V3E debugger/programmer with USB re-enumeration capability:
  - Mass storage
  - Virtual COM port
  - Debug port
- 4 user LEDs
- User and reset push-buttons

## Hardware Specifications

The STM32H573xx devices are high-performance microcontrollers (STM32H5 Series) based on the high-performance Arm® Cortex®-M33 32-bit RISC core. They operate at a frequency of up to 250 MHz.

### Core

- ARM® 32-bit Cortex® -M33 CPU with TrustZone® and FPU
- Performance benchmark: 375 DMPIS/MHz (Dhrystone 2.1)

### Security Features

- Arm® TrustZone® with ARMv8-M mainline security extension
- Up to 8 configurable SAU regions
- TrustZone® aware and securable peripherals
- Flexible lifecycle scheme with secure debug authentication
- Preconfigured immutable root of trust (ST-iROT)
- SFI (secure firmware installation)
- Secure data storage with hardware unique key (HUK)
- Secure firmware upgrade support with TF-M
- 2x AES coprocessors including one with DPA resistance
- Public key accelerator, DPA resistant
- On-the-fly decryption of Octo-SPI external memories
- HASH hardware accelerator
- True random number generator, NIST SP800-90B compliant
- 96-bit unique ID
- Active tampers

### Clock Management

- 25 MHz crystal oscillator (HSE)
- 32 kHz crystal oscillator for RTC (LSE)
- Internal 64 MHz (HSI) trimmable by software
- Internal low-power 32 kHz RC (LSI)( ±5%)
- Internal 4 MHz oscillator (CSI), trimmable by software
- Internal 48 MHz (HSI48) with recovery system
- 3 PLLs for system clock, USB, audio, ADC

### Power Management

- Embedded regulator (LDO) with three configurable range output to supply the digital circuitry
- Embedded SMPS step-down converter
- RTC with HW calendar, alarms and calibration

### I/O and Peripherals

- Up to 139 fast I/Os, most 5 V-tolerant, up to 10 I/Os with independent supply down to 1.08 V
- Up to 16 timers and 2 watchdogs:
  - 12x 16-bit
  - 2x 32-bit timers with up to 4 IC/OC/PWM or pulse counter and quadrature (incremental) encoder input
  - 6x 16-bit low-power 16-bit timers (available in Stop mode)
  - 2x watchdogs
  - 2x SysTick timer

### Memory

- Up to 2 MB Flash, 2 banks read-while-write
- 1 Kbyte OTP (one-time programmable)
- 640 KB of SRAM including 64 KB with hardware parity check and 320 Kbytes with flexible ECC
- 4 Kbytes of backup SRAM available in the lowest power modes
- Flexible external memory controller with up to 16-bit data bus: SRAM, PSRAM, FRAM, SDRAM/LPSDR SDRAM, NOR/NAND memories
- 1x OCTOSPI memory interface with on-the-fly decryption and support for serial PSRAM/NAND/NOR, Hyper RAM/Flash frame formats
- 2x SD/SDIO/MMC interfaces

### Analog Peripherals

- 2x 12-bit ADC with up to 5 MSPS in 12-bit
- 2x 12-bit D/A converters
- 1x Digital temperature sensor

### Communication Interfaces

- 1x USB Type-C / USB power-delivery controller
- 1x USB 2.0 full-speed host and device
- 4x I2C FM+ interfaces (SMBus/PMBus)
- 1x I3C interface
- 12x U(S)ARTS (ISO7816 interface, LIN, IrDA, modem control)
- 1x LP UART
- 6x SPIs including 3 muxed with full-duplex I2S
- 5x additional SPI from 5x USART when configured in Synchronous mode
- 2x SAI
- 2x FDCAN
- 1x SDMMC interface
- 2x 16 channel DMA controllers
- 1x 8- to 14- bit camera interface
- 1x HDMI-CEC
- 1x Ethernel MAC interface with DMA controller
- 1x 16-bit parallel slave synchronous-interface

### Mathematical Accelerators

- CORDIC for trigonometric functions acceleration
- FMAC (filter mathematical accelerator)
- CRC calculation unit

### Development Support

- Serial wire debug (SWD)
- JTAG
- Embedded Trace Macrocell™

## Supported Features

The STM32H573I-DK board supports the following hardware features:

### CPU

- ARM Cortex-M33 CPU

### ADC

- STM32 ADC1

### CAN

- STM32 FDCAN CAN FD controller

### Clock Control

- STM32U5 RCC (Reset and Clock controller)
- STM32 HSE Clock
- STM32 HSI Clock
- Generic fixed-rate clock provider
- STM32 LSE Clock
- STM32U5 PLL
- STM32 Microcontroller Clock Output (MCO)

### Counter

- STM32 counters

### Cryptographic Accelerator

- STM32 AES Accelerator

### DAC

- STM32 family DAC

### DMA

- STM32U5 DMA controller

### Ethernet

- STM32H7 Ethernet
- Generic MII PHY

### Flash Controller

- STM32 Family flash controller
- STM32 XSPI Flash controller supporting the JEDEC CFI interface

### GPIO & Headers

- STM32 GPIO controller
- GPIO pins exposed on Arduino Uno (R3) headers

### I2C

- STM32 I2C V2 controller

### I2S

- STM32H7 I2S controller

### I3C

- STM32H5 I3C controller

### Input

- Group of GPIO-bound input keys

### Interrupt Controller

- ARMv8-M NVIC (Nested Vectored Interrupt Controller)
- STM32G0 External Interrupt Controller

### LED

- Group of GPIO-controlled LEDs

### MDIO

- STM32 MDIO Controller

### Memory Controller

- STM32 Flexible Memory Controller (FMC)

### MMC

- STM32 SDMMC Disk Access

### MMU / MPU

- ARMv8-M MPU (Memory Protection Unit)

### MTD

- STM32 flash memory
- Fixed partitions of a flash (or other non-volatile storage) memory

### PHY

- This binding is to be used by all the usb transceivers which are built-in with USB IP

### Pin Control

- STM32 Pin controller

### PWM

- STM32 PWM

### Reset Controller

- STM32 Reset and Clock Control (RCC) Controller

### RNG

- STM32 Random Number Generator

### RTC

- STM32 RTC

### Sensors

- STM32 Digital Temperature Sensor
- STM32 family TEMP node for production calibrated sensors with two calibration temperatures
- STM32 VREF+
- STM32 VBAT

### Serial Controller

- STM32 USART
- STM32 LPUART
- STM32 UART

### SMbus

- STM32 SMBus controller

### SPI

- STM32H7 SPI controller

### Timer

- ARMv8-M System Tick
- STM32 low-power timer (LPTIM)
- STM32 timers

### USB

- STM32 USB controller

### Watchdog

- STM32 watchdog
- STM32 system window watchdog

### xSPI

- STM32 XSPI Controller

## Default Zephyr Peripheral Mapping

- USART_1 TX/RX : PA9/PA10 (VCP)
- USART_3 TX/RX : PB11/PB10 (Arduino USART3)
- USER_PB : PC13
- LD1 (green) : PI9
- DAC1 channel 1 output : PA4
- ADC1 channel 6 input : PF12

## System Clock

STM32H573I-DK System Clock could be driven by internal or external oscillator, as well as main PLL clock. By default System clock is driven by PLL clock at 240MHz, driven by 25MHz external oscillator (HSE).

## Serial Port

STM32H573I-DK Discovery board has 3 U(S)ARTs. The Zephyr console output is assigned to USART1. Default settings are 115200 8N1.

## Programming and Debugging

STM32H573I-DK Discovery board includes an ST-LINK/V3E embedded debug tool interface.

### OpenOCD Support

For now, OpenOCD support for STM32H5 is not available on upstream OpenOCD. You can check OpenOCD official Github mirror. In order to use it though, you should clone from the customized STMicroelectronics OpenOCD Github and compile it following usual README guidelines. Once it is done, you can set the OPENOCD and OPENOCD_DEFAULT_PATH variables in boards/st/stm32h573i_dk/board.cmake to point the build to the paths of the OpenOCD binary and its scripts, before including the common openocd.board.cmake file:

```cmake
set(OPENOCD "<path_to_openocd_repo>/src/openocd" CACHE FILEPATH "" FORCE)
set(OPENOCD_DEFAULT_PATH <path_to_opneocd_repo>/tcl)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
```

### Flashing

The board is configured to be flashed using west STM32CubeProgrammer runner, so its installation is required.

Alternatively, OpenOCD or pyOCD can also be used to flash the board using the --runner (or -r) option:

```bash
west flash --runner openocd
west flash --runner pyocd
```

For pyOCD, additional target information needs to be installed by executing the following commands:

```bash
pyocd pack --update
pyocd pack --install stm32h5
```

### Flashing an Application

Connect the STM32H573I-DK Discovery to your host computer using the USB port. Then build and flash an application. Here is an example for the Hello World application.

Run a serial host program to connect with your Nucleo board:

```bash
minicom -D /dev/ttyACM0
```

Then build and flash the application.

```bash
# From the root of the zephyr repository
west build -b stm32h573i_dk samples/hello_world
west flash
```

You should see the following message on the console:

```
Hello World! stm32h573i_dk
```

### Debugging

Waiting for OpenOCD support, debugging could be performed with pyOCD which requires to enable "pack" support with the following pyOCD command:

```bash
pyocd pack --update
pyocd pack --install stm32h5
```

Once installed, you can debug an application in the usual way. Here is an example for the Hello World application.

```bash
# From the root of the zephyr repository
west build -b stm32h573i_dk samples/hello_world
west debug
```

## Further Reading

- [STM32H573 on www.st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32h573.html)
- [STM32H573 reference manual](https://www.st.com/resource/en/reference_manual/rm0481-stm32h563h573-and-stm32h562h572-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32H573I-DK Discovery website](https://www.st.com/en/evaluation-tools/stm32h573i-dk.html)
