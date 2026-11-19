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

## The device keystore

`robohub/keystore.h` is where a board keeps the device key it was rotated to
(REKEY). Two functions:

```c
typedef struct { uint32_t epoch; uint8_t scope[8]; uint8_t key[16]; } rh_key_t;
int rh_keystore_load(rh_key_t *out);         /* 0 found, -1 none */
int rh_keystore_store(const rh_key_t *key);  /* 0 stored and verified, -1 not */
```

When load finds nothing the firmware uses its compiled-in key and
`RH_DEVICE_KEY_EPOCH`. This module carries weak defaults that answer -1, so a
platform without a store still links and a REKEY is answered "cannot rotate".

`robohub/keystore_core.h` is the logic every platform shares: two A/B slots of
one 40-byte record each (magic `RHK1`, a sequence number, the epoch, an opaque 8-byte scope, the key, a
CRC32). A write goes to the slot that does not hold the newest valid record,
with the next sequence number, and counts only once it has been read back and
compared; a torn write, a flipped bit or an erased slot fails its CRC and is
ignored, so power lost mid-write leaves the previous key in place. Sequence
numbers compare modulo 2^32, so the counter wraps without a stale key winning.
A platform supplies only a backend that reads, erases and writes one slot:

| Platform | Where the slots are |
|---|---|
| robohub-pico | the last two 4 KiB flash sectors; the link fails if the image reaches them |
| robohub-esp32 | NVS namespace `rh_keys`, blobs `a` and `b` |
| robohub-stm32 | the last two bank-2 sectors on boards that reserve them (NUCLEO-H753ZI); the weak default elsewhere |
| robohub-avr | the 80 bytes of EEPROM below the nonce counter |
| robohub-linux | one 0600 file, replaced atomically |

Every platform keystore object also defines `rh_keystore_platform`, which
nothing else defines, and the platform's build names it undefined on the link
(`--undefined=rh_keystore_platform`). That pulls the object out of the platform
archive whatever the archive order, so its strong functions always replace the
weak defaults; a static-archive link cannot quietly settle for "no store".

`tests/test_keystore.c` runs the core over an in-memory backend that loses power
on cue.

## Production builds

`CONFIG_RH_PRODUCTION` (declared in this module's `Kconfig`, off by default)
asks a platform for its production preset. Building with it burns nothing; the
irreversible steps are listed, and confirmed by typing the board's name, by
`rho flash --production`. Each platform's docs say what its preset does.
