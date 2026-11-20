# Changelog

## [1.1.0] - 2026-11-14

A keystore contract (`robohub/keystore.h`): two A/B slots with a sequence and a CRC, so a write cut short keeps the previous key; a platform supplies the raw storage. `CONFIG_RH_PRODUCTION` declares a production build, off by default.

## [1.1.1] - 2026-11-19

A connector's `position` in the board contract (`contracts/module/defs.v1.json`) may be a number, as every board manifest writes it (`position: 1`); the schema allowed only a string, so a strict validator refused 39 boards.

## [1.2.0] - 2026-11-20

The device contract admits `device.kind: coordinator` — a radio a host drives, such as a Zigbee USB dongle, which never crosses the wire and so has no `RoboHubDeviceKind` number — and no longer requires a `build` section, so a device read only from its manifest (an MQTT or Zigbee device, a coordinator) validates without one (COMM-0009). `contracts/module-types.json`, `module/defs.v1.json` and `module/types/device.v1.json` say the same.

## [1.3.0] - 2026-11-20

A health contract (`robohub/health.h`, COMM-0010..0012): the watchdog a platform has (or plainly has not), the reason of the last reset read once from the platform's flags, and a fault record with a boot count in memory a reset keeps, written by a fault handler and taken once at the next boot. Weak defaults keep a platform without the hardware linking: no watchdog, reason unknown, every boot a power-on.

## [1.4.0] - 2026-11-20

An update slot contract (`robohub/update_slot.h`, COMM-0013, robohub ADR-0022): whether a platform takes images over the link and how large its inactive slot is; begin, write in order, read back; boot once, keeping the update's id and versions with it; on the next boot, pending or not, confirm, and what became of the last image booted once (kept or rolled back, taken once); the board's security version; which slot runs. Weak defaults answer "not updatable".
