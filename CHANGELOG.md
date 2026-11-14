# Changelog

## [1.1.0] - 2026-11-14

A keystore contract (`robohub/keystore.h`): two A/B slots with a sequence and a CRC, so a write cut short keeps the previous key; a platform supplies the raw storage. `CONFIG_RH_PRODUCTION` declares a production build, off by default.
