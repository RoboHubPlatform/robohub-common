/**
 * @file keystore_core.h
 * @brief The portable half of a keystore: A/B slots, CRC32, sequence numbers.
 *
 * Every platform's keystore is the same logic over a different medium — flash
 * sectors on a Pico or an STM32, EEPROM on an AVR, NVS on an ESP32, a file on a
 * Linux host. This is that logic, written once; a platform supplies a backend
 * that reads, erases and writes one slot of its reserved region, and defines
 * rh_keystore_load/rh_keystore_store (robohub/keystore.h) as calls into it.
 *
 * A slot holds one record of RH_KEYSTORE_RECORD_SIZE bytes, little-endian:
 *
 *     offset  size  field
 *          0     4  magic "RHK1"
 *          4     4  sequence number (monotonic, compared modulo 2^32)
 *          8     4  epoch
 *         12     8  scope (opaque, stored as given)
 *         20    16  key
 *         36     4  CRC32 (IEEE, reflected) of bytes 0..35
 *
 * A slot whose magic or CRC does not match — erased flash, a torn write, a
 * flipped bit — is ignored. Of two valid slots the one with the newer sequence
 * number wins, the comparison being serial-number arithmetic so the counter
 * wraps from 0xFFFFFFFF to 0 without a stored key going stale.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_KEYSTORE_CORE_H
#define INCLUDE_ROBOHUB_KEYSTORE_CORE_H

#include <stddef.h>
#include <stdint.h>

#include "robohub/keystore.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Defined (strongly, with no default anywhere) by every platform keystore
 * object. A platform's build names it as an undefined symbol on the link
 * (`--undefined=rh_keystore_platform`), which forces the linker to take that
 * object out of the platform's archive whatever the archive order, and with it
 * the strong rh_keystore_load/rh_keystore_store that override the weak
 * defaults in robohub-common.
 */
extern const char rh_keystore_platform[];

/** The bytes one slot's record takes; a backend's slot is at least this big. */
#define RH_KEYSTORE_RECORD_SIZE 40u

/** The number of slots: A and B. */
#define RH_KEYSTORE_SLOTS 2u

/**
 * One platform's raw access to its reserved region, slot by slot.
 *
 * `read` copies `len` bytes from the start of `slot` into `buf`; `erase` puts the
 * slot back to its erased state (may be NULL for a medium that overwrites in
 * place, such as EEPROM or a file); `write` stores `len` bytes at the start of the
 * erased slot. Each answers 0 on success and non-zero on failure. `ctx` is passed
 * back to each call unchanged.
 */
typedef struct rh_keystore_backend {
    void *ctx;
    int (*read)(void *ctx, unsigned slot, uint8_t *buf, size_t len);
    int (*erase)(void *ctx, unsigned slot);
    int (*write)(void *ctx, unsigned slot, const uint8_t *buf, size_t len);
} rh_keystore_backend_t;

/** rh_keystore_load over a backend. 0 when a valid record was found, else -1. */
int rh_keystore_core_load(const rh_keystore_backend_t *backend, rh_key_t *out);

/** rh_keystore_store over a backend: write the slot not holding the newest
 *  record with the next sequence number, read it back and compare. 0 when
 *  stored, -1 when any step failed (the previous key is then still the newest). */
int rh_keystore_core_store(const rh_keystore_backend_t *backend, const rh_key_t *key);

/** CRC32 (IEEE 802.3, reflected, as zlib computes it) of `len` bytes. */
uint32_t rh_keystore_crc32(const uint8_t *data, size_t len);

/** Encode a record; `out` holds RH_KEYSTORE_RECORD_SIZE bytes. */
void rh_keystore_encode(uint8_t *out, uint32_t sequence, const rh_key_t *key);

/** Decode a record. 0 when its magic and CRC hold, else -1. */
int rh_keystore_decode(const uint8_t *record, uint32_t *sequence, rh_key_t *key);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_KEYSTORE_CORE_H */
