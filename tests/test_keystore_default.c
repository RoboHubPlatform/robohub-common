/*
 * test_keystore_default.c — the keystore with no platform: nothing stored,
 * nothing can be, and the program still links.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/keystore.h"

#include <assert.h>

int main(void)
{
    rh_key_t key = { 1u, { 0 }, { 0 } };
    assert(rh_keystore_load(&key) == -1);
    assert(rh_keystore_store(&key) == -1);
    return 0;
}
