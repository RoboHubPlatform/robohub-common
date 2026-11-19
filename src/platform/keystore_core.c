/**
 * @file keystore_core.c
 * @brief A/B slots, CRC32 and sequence numbers, over any platform's backend.
 *
 * See robohub/keystore_core.h for the record layout. The rules that keep the
 * previous key safe:
 *
 *  - a write never touches the slot holding the newest valid record;
 *  - the new record's sequence number is the newest one's plus one, so it only
 *    wins once it is completely written and its CRC holds;
 *  - a write counts only after it has been read back and compared.
 *
 * Stack copies of key material are wiped before returning.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "robohub/keystore_core.h"

#include <string.h>

static const uint8_t rh_keystore_magic[4] = { 'R', 'H', 'K', '1' };

static void rh_keystore_wipe(void *buf, size_t len)
{
    volatile uint8_t *p = (volatile uint8_t *)buf;
    while (len--) {
        *p++ = 0;
    }
}

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

// @req COMM-0006
uint32_t rh_keystore_crc32(const uint8_t *data, size_t len)
{
    /* Bitwise rather than table-driven: 40 bytes are checked at boot and on a
     * rekey, and a 1 KiB table is a lot of an ATmega's flash. */
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc >> 1) ^ (0xEDB88320u & (uint32_t)(-(int32_t)(crc & 1u)));
        }
    }
    return ~crc;
}

void rh_keystore_encode(uint8_t *out, uint32_t sequence, const rh_key_t *key)
{
    memcpy(out, rh_keystore_magic, 4);
    put32(out + 4, sequence);
    put32(out + 8, key->epoch);
    memcpy(out + 12, key->scope, 8);
    memcpy(out + 20, key->key, 16);
    put32(out + 36, rh_keystore_crc32(out, 36));
}

// @req COMM-0006
int rh_keystore_decode(const uint8_t *record, uint32_t *sequence, rh_key_t *key)
{
    if (memcmp(record, rh_keystore_magic, 4) != 0) {
        return -1;
    }
    if (get32(record + 36) != rh_keystore_crc32(record, 36)) {
        return -1;
    }
    if (sequence != 0) {
        *sequence = get32(record + 4);
    }
    if (key != 0) {
        key->epoch = get32(record + 8);
        memcpy(key->scope, record + 12, 8);
        memcpy(key->key, record + 20, 16);
    }
    return 0;
}

/* Which slot holds the newest valid record: its index, or -1 when neither is
 * valid. `sequence` and `key` receive that record's fields. */
static int rh_keystore_newest(const rh_keystore_backend_t *backend, uint32_t *sequence, rh_key_t *key)
{
    int newest = -1;
    uint32_t newest_sequence = 0;
    uint8_t record[RH_KEYSTORE_RECORD_SIZE];
    rh_key_t candidate;

    for (unsigned slot = 0; slot < RH_KEYSTORE_SLOTS; slot++) {
        uint32_t s = 0;
        if (backend->read(backend->ctx, slot, record, sizeof(record)) != 0) {
            continue;
        }
        if (rh_keystore_decode(record, &s, &candidate) != 0) {
            continue;
        }
        /* Serial-number arithmetic: newer when the difference, read as a signed
         * 32-bit value, is positive. Two slots are only ever one apart, so the
         * comparison holds across the wrap from 0xFFFFFFFF to 0. */
        if (newest < 0 || (int32_t)(s - newest_sequence) > 0) {
            newest = (int)slot;
            newest_sequence = s;
            if (key != 0) {
                *key = candidate;
            }
        }
    }
    if (sequence != 0) {
        *sequence = newest_sequence;
    }
    rh_keystore_wipe(record, sizeof(record));
    rh_keystore_wipe(&candidate, sizeof(candidate));
    return newest;
}

// @req COMM-0006
int rh_keystore_core_load(const rh_keystore_backend_t *backend, rh_key_t *out)
{
    if (backend == 0 || backend->read == 0 || out == 0) {
        return -1;
    }
    rh_key_t found;
    int slot = rh_keystore_newest(backend, 0, &found);
    if (slot < 0) {
        return -1;
    }
    *out = found;
    rh_keystore_wipe(&found, sizeof(found));
    return 0;
}

// @req COMM-0006
int rh_keystore_core_store(const rh_keystore_backend_t *backend, const rh_key_t *key)
{
    if (backend == 0 || backend->read == 0 || backend->write == 0 || key == 0) {
        return -1;
    }

    uint32_t sequence = 0;
    int newest = rh_keystore_newest(backend, &sequence, 0);
    unsigned target = (newest == 0) ? 1u : 0u;
    uint32_t next = (newest < 0) ? 1u : sequence + 1u;

    uint8_t record[RH_KEYSTORE_RECORD_SIZE];
    uint8_t check[RH_KEYSTORE_RECORD_SIZE];
    int result = -1;

    rh_keystore_encode(record, next, key);
    if (backend->erase != 0 && backend->erase(backend->ctx, target) != 0) {
        goto done;
    }
    if (backend->write(backend->ctx, target, record, sizeof(record)) != 0) {
        goto done;
    }
    if (backend->read(backend->ctx, target, check, sizeof(check)) != 0) {
        goto done;
    }
    if (memcmp(record, check, sizeof(record)) != 0) {
        goto done;
    }
    result = 0;

done:
    rh_keystore_wipe(record, sizeof(record));
    rh_keystore_wipe(check, sizeof(check));
    return result;
}
