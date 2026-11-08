# STM32-PLL-Config

System Clock configuration for STM32F103RB (Nucleo-F103RB) to run at 72 MHz, written with direct register access — no HAL, no CubeMX code. 

This is my foundational learning project: I wanted to understand the Clock Tree of the STM32 (HSE, PLL, prescalers for AHB, APB1, APB2) and how the Flash memory latency must be adjusted before increasing the system clock.

## Hardware

- Board: NUCLEO-F103RB
- System Clock: 72 MHz (HSE 8 MHz -> PLL x9)
- AHB Bus: 72 MHz
- APB1 Bus: 36 MHz (Max allowed for APB1)
- APB2 Bus: 72 MHz
- LED: LD2, connected to PA5 (used for visual verification)

## Requirements

- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (`arm-none-eabi-gcc` in PATH)
- CMake ≥ 3.22
- GNU Make (e.g. via MinGW)
- [OpenOCD](https://openocd.org/) with ST-LINK support
- ST-LINK USB driver (Windows)

## Build

```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
openocd -f flash.cfg -c "program build/pll_registers.hex verify reset exit"
