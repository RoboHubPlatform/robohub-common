/**
 * @file update_slot.h
 * @brief The update slot: where a new firmware image is written, how it is
 *        booted once, and how it is kept or rolled back (COMM-0013).
 *
 * Each chip's own A/B mechanism sits behind this contract (robohub ADR-0021,
 * ADR-0022): the RP2350's partitions and try-before-you-buy, ESP-IDF's OTA slots,
 * MCUboot, two files on a Linux host. The managed layer of robohub-node drives it
 * and never knows which one it is talking to.
 *
 * The life of an image:
 *
 *   rh_update_begin(size)          the inactive slot is made ready for `size` bytes
 *   rh_update_write(offset, ...)   written in order, never the running slot
 *   rh_update_read(offset, ...)    read back, to hash what was written
 *   rh_update_boot_once(&info)     marked to boot once, and the board restarts
 *   -- next boot --
 *   rh_update_pending(&info)       true: this image was booted once, unconfirmed
 *   rh_update_confirm()            kept: the board boots it from now on, and its
 *                                  security version becomes the board's
 *   rh_update_reject()             or given up: the board restarts into the old one
 *   -- or the board resets before confirming --
 *   rh_update_take_outcome(&info)  RH_UPDATE_ROLLED_BACK, once
 *
 * A platform with no second image links the weak defaults here: not updatable,
 * and every call says so rather than failing in some other way.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_UPDATE_SLOT_H
#define INCLUDE_ROBOHUB_UPDATE_SLOT_H

#include <stdbool.h>
#include <stdint.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/** What the platform can take. All zero when it cannot take images at all. */
typedef struct {
    bool     updatable;   /**< it has an inactive slot to write */
    uint32_t slot_size;   /**< the largest image that fits there, in bytes */
} rh_update_caps;

/** What a platform keeps about an image booted once, until it is kept or rolled back. */
typedef struct {
    uint32_t update_id;         /**< the manifest's update id */
    uint16_t firmware_version;  /**< the image's firmware version */
    uint32_t security_version;  /**< the image's security version */
} rh_update_info;

/** What became of the last image booted once. */
typedef enum {
    RH_UPDATE_NONE        = 0,  /**< nothing to report */
    RH_UPDATE_KEPT        = 1,  /**< it was confirmed */
    RH_UPDATE_ROLLED_BACK = 2,  /**< the board reset before confirming it */
} rh_update_outcome;

/** What this platform can take. The weak default: nothing. */
void rh_update_caps_get(rh_update_caps *caps);

/**
 * Make the inactive slot ready for an image of `size` bytes, forgetting any
 * image half-written there. ROBOHUB_ERR_NOT_SUPPORTED when the platform is not
 * updatable; ROBOHUB_ERR_NO_MEM when the image does not fit.
 */
RoboHubError rh_update_begin(uint32_t size);

/**
 * Write `length` bytes at `offset` of the inactive slot. Offsets come in order,
 * each where the last write ended. ROBOHUB_ERR_STATE without a begin, or out of
 * order; ROBOHUB_ERR_INVALID_ARG past the size begun.
 */
RoboHubError rh_update_write(uint32_t offset, const uint8_t *data, uint32_t length);

/** Read `length` bytes at `offset` of what was written. */
RoboHubError rh_update_read(uint32_t offset, uint8_t *out, uint32_t length);

/**
 * Mark the whole image written to boot once, keep `info` with it, and restart
 * the board into it. Returns only when it could not: ROBOHUB_ERR_STATE when the
 * image was not written whole.
 */
RoboHubError rh_update_boot_once(const rh_update_info *info);

/**
 * True when the image running was booted once and is not confirmed yet; `out`
 * (when not NULL) is what was kept with it.
 */
bool rh_update_pending(rh_update_info *out);

/**
 * Keep the image running: the board boots it from now on, and its security
 * version becomes the board's. ROBOHUB_ERR_STATE when nothing is pending.
 */
RoboHubError rh_update_confirm(void);

/**
 * Give up the image running on trial (COMM-0014): the board restarts into the
 * image it ran before, and the next boot's outcome is RH_UPDATE_ROLLED_BACK.
 * Returns only when it could not: ROBOHUB_ERR_STATE when nothing is pending.
 */
RoboHubError rh_update_reject(void);

/**
 * What became of the last image booted once, taken once: a second call answers
 * RH_UPDATE_NONE. `out` (when not NULL) is what was kept with it.
 */
rh_update_outcome rh_update_take_outcome(rh_update_info *out);

/** The board's security version: an image below it is refused. 0 when none was kept. */
uint32_t rh_update_security_version(void);

/** Which slot runs (0 or 1), or -1 when the platform cannot say. */
int rh_update_running_slot(void);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_UPDATE_SLOT_H */
