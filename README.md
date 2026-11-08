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
```

Produces `build/pll_registers.hex` and `build/pll_registers.bin`.

## Flash

```bash
openocd -f flash.cfg -c "program build/pll_registers.hex verify reset exit"
```

## Project layout

- `src/main.c` — main application: Clock configuration, direct register access for GPIO toggling
- `src/system_stm32f1xx.c` — CMSIS system init
- `startup/startup_stm32f103xb.s` — startup code (GCC syntax)
- `linker/STM32F103RBTx_FLASH.ld` — linker script
- `CMSIS/` — CMSIS core and device headers
- `flash.cfg` — OpenOCD config

## Implementation Details

The `SystemClock_Config` function performs the following steps:
1. **Enable HSE**: Turns on the external 8 MHz crystal and waits for the `HSERDY` flag with a timeout to prevent hanging.
2. **Flash Latency**: Sets 2 wait states (`FLASH_ACR_LATENCY_2`) and enables the prefetch buffer. This is strictly required for running at 72 MHz.
3. **Prescalers Setup**: Configures AHB = 72 MHz, APB1 = 36 MHz, APB2 = 72 MHz.
4. **PLL Setup**: Selects HSE as the PLL source, configures the multiplier to x9 (8 MHz * 9 = 72 MHz).
5. **Switch System Clock**: Enables the PLL, waits for `PLLRDY`, then switches the system clock source (`SW`) to PLL and waits for the status flag (`SWS`) to confirm.

*Note: All waiting loops are protected by a timeout counter (`HSE_STARTUP_TIMEOUT`) to prevent the MCU from locking up if the external crystal fails to start.*
