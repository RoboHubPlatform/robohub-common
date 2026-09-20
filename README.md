# RoboHub Common
Core Interfaces & Platform Abstractions for RoboHub Platform  
Version 1.0.0  
License: MIT

## Overview
RoboHub Common is the core, platform-independent module of the RoboHub Platform.
It defines the foundational interfaces, abstractions, and contracts required by all higher-level modules, drivers, and hardware platforms.

This module contains no hardware-specific code.
Instead, it provides the abstract layers that every platform (Pico, ESP32, STM32, Linux) must implement through their own HAL implementations and board registration logic.

RoboHub Common is always enabled and required by the entire ecosystem.

## Features

### Core Interfaces
- Unified type system (`rh_u8`, `rh_size`, `rh_name`, etc.)
- Centralized error model (`RoboHubError`)
- Hardware Abstraction Layer (HAL) interfaces:
  - GPIO
  - I2C
  - SPI
  - UART
  - RS485
  - Timer

### Platform Abstractions
- Platform initialization contract
- Board registration contract
- Multi-instance HAL registry (e.g. `I2C0`, `SPI1`, `UART_MAIN`)

## Directory Structure

```text
robohub-common/
├── include/
│   ├── types.h
│   ├── error.h
│   ├── platform.h
│   ├── registry.h
│   └── robohub/
│       ├── hal/
│       ├── drivers/
│       └── devices/
│
├── src/
│   ├── error.c
│   └── platform/
│       ├── platform.c
│       └── registry.c
│
├── kconfig/
│   └── hal/
│       ├── Kconfig.gpio
│       ├── Kconfig.i2c
│       ├── Kconfig.spi
│       ├── Kconfig.uart
│       ├── Kconfig.rs485
│       └── Kconfig.timer
│
├── cmake/
│   ├── Kconfig.cmake
│   └── kconfig_include.cmake
│
├── tools/
│   └── kconfig/
│       └── kconfig_gen.py
│
├── tests/
│   ├── test_error.c
│   ├── test_platform.c
│   └── test_registry.c
│
├── module.yml
└── LICENSE
```

## HAL Configuration (Kconfig)

HAL configuration is split into modular Kconfig fragments under:

kconfig/hal/

Each HAL interface has its own Kconfig file, for example:

```kconfig
kconfig/hal/Kconfig.gpio:

menu "HAL GPIO"

config RH_HAL_GPIO
    bool "Enable HAL GPIO interface"
    default y

endmenu
```

### Selecting HAL Interfaces in an Application

In your application configuration (e.g. project.conf):

```text
CONFIG_RH_HAL_GPIO=y
CONFIG_RH_HAL_I2C=n
CONFIG_RH_HAL_SPI=y
CONFIG_RH_HAL_UART=n
CONFIG_RH_HAL_RS485=n
CONFIG_RH_HAL_TIMER=y
```

Effects:
- The generated `kconfig.h` exposes the selected values as `CONFIG_*` macros.
- Platform modules (e.g. robohub-pico) can compile only the enabled HAL implementations.
- Platform registration code can register only the enabled HALs.

## Build & Integration

RoboHub Common is built as a static library and exports its public headers.

### CMake Integration

```cmake
add_subdirectory(robohub-common)
target_link_libraries(your_module PUBLIC robohub-common)
```

The build requires CMake 3.16 or newer, a C11 compiler, and Python 3 for
Kconfig generation. An application configuration can be supplied with
`-DROBOHUB_KCONFIG_FILE=/path/to/project.conf`.

### Kconfig Integration

RoboHub Common uses:

- cmake/Kconfig.cmake to integrate Kconfig into CMake
- tools/kconfig/kconfig_gen.py to generate generated/kconfig.h
- kconfig/hal/* as HAL configuration fragments

Include the generated header directory:

include_directories(${CMAKE_BINARY_DIR}/generated)

Use configuration flags in code:

```c
#if CONFIG_RH_HAL_GPIO
    // GPIO interface enabled
#endif
```

## Continuous Integration

GitHub Actions runs on pushes and pull requests. The workflow configures and
builds the library, runs the CTest suite, verifies generated Kconfig values,
and checks the installed headers and static library.

## Purpose in the Ecosystem

RoboHub Common is the foundation layer for the entire RoboHub Platform:

- All hardware platforms depend on it.
- All drivers depend on it.
- Higher-level modules and orchestrators depend on it.
- The registry depends on it.

It is effectively always present in any RoboHub-based system.

## License

This module is released under the MIT License.  
See the LICENSE file for details.

## Maintainer

Aurel Miron  
RoboHub Platform Architect  
Cluj-Napoca, Romania
