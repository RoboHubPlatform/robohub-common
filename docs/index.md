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

## Module contracts

The ecosystem's module types are declared here, not in the toolchain that reads
the manifests. `contracts/module-types.json` is the contract the Rho toolchain
loads through the `rho-robohub` plugin: the allowed `module.type` words, which
of them are native, and the sections a manifest of each type must carry.

The published schemas live beside it, as a core schema and one schema per type
that extends it:

- `contracts/module/core.v1.json` — what every module says before its type says
  anything else;
- `contracts/module/types.v1.json` — the `module.type` enum and the schema each
  type is written to;
- `contracts/module/types/<type>.v1.json` — one type's structure, extending the
  core;
- `contracts/module/defs.v1.json` — the structures more than one type shares,
  such as a device, a signal and a board.

A module of a type the toolchain does not define is validated against this
contract when its platform is recognized. A device is a `device`, not a
`driver`: a driver reaches a part through the HAL, and a device reaches the
equipment that part is in.

The `device.kind` values are the manifest's spelling of `RoboHubDeviceKind` in
`include/robohub/devices/device.h`: the wire carries the number, a manifest says
the word, and the two lists are one vocabulary.

Every manifest carries a `module.id`: its stable identity, prefixed by the
repository and path it lives under. A name is what a person reads; an id is what
a consumer keys on. A signal source is `<module.id>.<signal.id>` within a
device instance, so two modules with the same name in different repositories are
two sources rather than one.
