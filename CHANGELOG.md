# Changelog

## [1.1.0] - 2026-11-14

A keystore contract (`robohub/keystore.h`): two A/B slots with a sequence and a CRC, so a write cut short keeps the previous key; a platform supplies the raw storage. `CONFIG_RH_PRODUCTION` declares a production build, off by default.

## [1.1.1] - 2026-11-19

A connector's `position` in the board contract (`contracts/module/defs.v1.json`) may be a number, as every board manifest writes it (`position: 1`); the schema allowed only a string, so a strict validator refused 39 boards.

## [1.2.0] - 2026-11-20

The device contract admits `device.kind: coordinator` — a radio a host drives, such as a Zigbee USB dongle, which never crosses the wire and so has no `RoboHubDeviceKind` number — and no longer requires a `build` section, so a device read only from its manifest (an MQTT or Zigbee device, a coordinator) validates without one (COMM-0009). `contracts/module-types.json`, `module/defs.v1.json` and `module/types/device.v1.json` say the same.
