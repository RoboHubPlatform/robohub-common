# RoboHub Common

The interfaces every platform implements: types, errors, the HAL, the device
registry. This is the contract that all platforms, devices, and drivers are
written against.

## The interfaces

- **Types**: the core types used across the ecosystem
- **Errors**: the error codes every layer agrees on
- **HAL**: the hardware abstraction layer (GPIO, I2C, SPI, UART, RS485,
  timer, console, network)
- **Device registry**: the registry of devices and their capabilities
- **Platform**: the platform abstraction

## The HAL

The HAL is the contract between a platform and the code that runs on it. A
platform implements the HAL; a device or driver uses it. The HAL is the same
on every platform, so a device written against the HAL runs on any platform
that implements it.
