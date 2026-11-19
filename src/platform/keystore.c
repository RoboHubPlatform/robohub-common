/**
 * @file keystore.c
 * @brief The keystore when a platform has none: nothing is stored.
 *
 * A platform with a store defines rh_keystore_load and rh_keystore_store
 * strongly and the linker prefers those. These weak defaults exist so that a
 * program on a platform without one still links: load finds no key, so the
 * firmware uses its compiled-in key and RH_DEVICE_KEY_EPOCH, and store refuses,
 * so a REKEY is answered with "cannot rotate". See robohub/keystore.h.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "robohub/keystore.h"

#if defined(__GNUC__) || defined(__clang__)
// @req COMM-0007
__attribute__((weak)) int rh_keystore_load(rh_key_t *out)
{
    (void)out;
    return -1;
}

// @req COMM-0007
__attribute__((weak)) int rh_keystore_store(const rh_key_t *key)
{
    (void)key;
    return -1;
}
#endif
