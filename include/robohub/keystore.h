/**
 * @file keystore.h
 * @brief Where a board keeps the device key it was rotated to.
 *
 * A board is built with its device key compiled in (robohub_secret.h, with
 * RH_DEVICE_KEY_EPOCH saying which epoch that key is). When the gateway rotates
 * the key (REKEY), the board stores the new key and its epoch here, and from its
 * next HELLO on it opens sessions with the stored key instead of the compiled-in
 * one.
 *
 * The store is two A/B slots, each holding one record with a CRC32 and a
 * monotonic sequence number. A write always goes to the slot that does not hold
 * the newest key, and is read back before it counts, so a power loss in the
 * middle of a write leaves the previous key where it was.
 *
 * A platform implements the store by defining these two functions strongly,
 * usually over the portable A/B core in robohub/keystore_core.h with a backend
 * that reads, erases and writes its own small reserved region. A platform that
 * has no store links the weak defaults in this module, which answer -1: the
 * firmware keeps its compiled-in key, and a REKEY is answered with "cannot
 * rotate".
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_KEYSTORE_H
#define INCLUDE_ROBOHUB_KEYSTORE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* `scope` is opaque to the keystore: stored and returned as is (and covered by
   the record's CRC). The secure layer uses it to tell which mesh a key belongs
   to, so a board moved to another mesh does not keep a key from its old one. */
typedef struct { uint32_t epoch; uint8_t scope[8]; uint8_t key[16]; } rh_key_t;

/* The newest valid stored key (two A/B slots, each with a CRC32 and a
   monotonic sequence number). 0 when found, -1 when none (the firmware then
   uses its compiled-in key and RH_DEVICE_KEY_EPOCH). */
int rh_keystore_load(rh_key_t *out);

/* Write `key` to the slot not holding the newest key, read it back and
   check it. 0 when stored, -1 when it could not be (or the platform has no
   store: the weak default implementation returns -1). */
int rh_keystore_store(const rh_key_t *key);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_KEYSTORE_H */
