/*
 * test_keystore.c — the A/B keystore core over an in-memory backend that can be
 * told to lose power part-way through a write.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/keystore_core.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define SLOT_BYTES 64u

typedef struct {
    uint8_t slot[RH_KEYSTORE_SLOTS][SLOT_BYTES];
    /* When >= 0, the next write stores only this many bytes and fails, the way a
     * board does when it loses power mid-program. */
    int cut_after;
    /* When set, the next erase happens and then power is lost before writing. */
    int fail_after_erase;
    int writes;
} memstore_t;

static int mem_read(void *ctx, unsigned slot, uint8_t *buf, size_t len)
{
    memstore_t *m = (memstore_t *)ctx;
    if (slot >= RH_KEYSTORE_SLOTS || len > SLOT_BYTES) return -1;
    memcpy(buf, m->slot[slot], len);
    return 0;
}

static int mem_erase(void *ctx, unsigned slot)
{
    memstore_t *m = (memstore_t *)ctx;
    if (slot >= RH_KEYSTORE_SLOTS) return -1;
    memset(m->slot[slot], 0xFF, SLOT_BYTES);
    if (m->fail_after_erase) {
        m->fail_after_erase = 0;
        return -1;
    }
    return 0;
}

static int mem_write(void *ctx, unsigned slot, const uint8_t *buf, size_t len)
{
    memstore_t *m = (memstore_t *)ctx;
    if (slot >= RH_KEYSTORE_SLOTS || len > SLOT_BYTES) return -1;
    m->writes++;
    if (m->cut_after >= 0) {
        size_t n = (size_t)m->cut_after < len ? (size_t)m->cut_after : len;
        memcpy(m->slot[slot], buf, n);
        m->cut_after = -1;
        return -1;
    }
    memcpy(m->slot[slot], buf, len);
    return 0;
}

static memstore_t store;
static const rh_keystore_backend_t backend = { &store, mem_read, mem_erase, mem_write };

static void reset(void)
{
    memset(&store, 0, sizeof(store));
    memset(store.slot, 0xFF, sizeof(store.slot)); /* erased flash */
    store.cut_after = -1;
}

static rh_key_t make_key(uint32_t epoch)
{
    rh_key_t k;
    k.epoch = epoch;
    for (int i = 0; i < 8; i++) k.scope[i] = (uint8_t)(0x50u + epoch + (uint32_t)i);
    for (int i = 0; i < 16; i++) k.key[i] = (uint8_t)(epoch * 16u + (uint32_t)i);
    return k;
}

static void expect_key(uint32_t epoch)
{
    rh_key_t got, want = make_key(epoch);
    memset(&got, 0, sizeof(got));
    assert(rh_keystore_core_load(&backend, &got) == 0);
    assert(got.epoch == epoch);
    assert(memcmp(got.scope, want.scope, 8) == 0); /* opaque, returned as stored */
    assert(memcmp(got.key, want.key, 16) == 0);
}

static uint32_t slot_sequence(unsigned slot)
{
    uint32_t seq = 0;
    assert(rh_keystore_decode(store.slot[slot], &seq, 0) == 0);
    return seq;
}

static void test_crc(void)
{
    /* The check value every CRC-32/ISO-HDLC implementation agrees on. */
    assert(rh_keystore_crc32((const uint8_t *)"123456789", 9) == 0xCBF43926u);
}

static void test_empty(void)
{
    reset();
    rh_key_t k;
    assert(rh_keystore_core_load(&backend, &k) == -1);
    /* All zeros is not a record either. */
    memset(store.slot, 0, sizeof(store.slot));
    assert(rh_keystore_core_load(&backend, &k) == -1);
}

static void test_alternates(void)
{
    reset();
    rh_key_t k1 = make_key(1), k2 = make_key(2), k3 = make_key(3);
    assert(rh_keystore_core_store(&backend, &k1) == 0);
    expect_key(1);
    assert(slot_sequence(0) == 1u);
    assert(rh_keystore_core_store(&backend, &k2) == 0);
    expect_key(2);
    assert(slot_sequence(1) == 2u);
    /* The third write replaces the oldest, never the newest. */
    assert(rh_keystore_core_store(&backend, &k3) == 0);
    expect_key(3);
    assert(slot_sequence(0) == 3u);
    assert(slot_sequence(1) == 2u);
}

static void test_power_loss_mid_write(void)
{
    rh_key_t k1 = make_key(1), k2 = make_key(2), k3 = make_key(3);
    for (int cut = 0; cut < (int)RH_KEYSTORE_RECORD_SIZE; cut++) {
        reset();
        assert(rh_keystore_core_store(&backend, &k1) == 0);
        assert(rh_keystore_core_store(&backend, &k2) == 0);
        store.cut_after = cut;
        assert(rh_keystore_core_store(&backend, &k3) == -1);
        expect_key(2); /* the previous key survives every torn write */
        /* and the store recovers: the next write succeeds */
        assert(rh_keystore_core_store(&backend, &k3) == 0);
        expect_key(3);
    }

    /* Power lost after the erase, before anything was written. */
    reset();
    assert(rh_keystore_core_store(&backend, &k1) == 0);
    assert(rh_keystore_core_store(&backend, &k2) == 0);
    store.fail_after_erase = 1;
    assert(rh_keystore_core_store(&backend, &k3) == -1);
    expect_key(2);

    /* The very first write torn: nothing valid, the compiled-in key is used. */
    reset();
    store.cut_after = 20;
    assert(rh_keystore_core_store(&backend, &k1) == -1);
    rh_key_t none;
    assert(rh_keystore_core_load(&backend, &none) == -1);
}

static void test_corrupt_slot_ignored(void)
{
    rh_key_t k1 = make_key(1), k2 = make_key(2);
    for (unsigned byte = 0; byte < RH_KEYSTORE_RECORD_SIZE; byte++) {
        reset();
        assert(rh_keystore_core_store(&backend, &k1) == 0);
        assert(rh_keystore_core_store(&backend, &k2) == 0);
        store.slot[1][byte] ^= 0x10; /* one flipped bit in the newest record */
        expect_key(1);
        /* The next store goes to the corrupt slot, not over the valid one. */
        rh_key_t k3 = make_key(3);
        assert(rh_keystore_core_store(&backend, &k3) == 0);
        expect_key(3);
        assert(slot_sequence(0) == 1u);
    }
}

static void test_sequence_wrap(void)
{
    rh_key_t k5 = make_key(5), k6 = make_key(6), k7 = make_key(7);

    /* Slot A at the top of the counter, slot B wrapped to 0: B is newer. */
    reset();
    rh_keystore_encode(store.slot[0], 0xFFFFFFFFu, &k5);
    rh_keystore_encode(store.slot[1], 0u, &k6);
    expect_key(6);
    assert(rh_keystore_core_store(&backend, &k7) == 0);
    expect_key(7);
    assert(slot_sequence(0) == 1u);

    /* The same, slots swapped. */
    reset();
    rh_keystore_encode(store.slot[1], 0xFFFFFFFFu, &k5);
    rh_keystore_encode(store.slot[0], 0u, &k6);
    expect_key(6);

    /* A store from the top of the counter wraps to 0 and wins. */
    reset();
    rh_keystore_encode(store.slot[0], 0xFFFFFFFEu, &k5);
    rh_keystore_encode(store.slot[1], 0xFFFFFFFFu, &k6);
    expect_key(6);
    assert(rh_keystore_core_store(&backend, &k7) == 0);
    assert(slot_sequence(0) == 0u);
    expect_key(7);
}

static void test_scope_is_covered_by_crc(void)
{
    reset();
    rh_key_t k1 = make_key(1), k2 = make_key(2);
    assert(rh_keystore_core_store(&backend, &k1) == 0);
    assert(rh_keystore_core_store(&backend, &k2) == 0);
    store.slot[1][12] ^= 0x01; /* a bit of the newest record's scope */
    expect_key(1);
}

static void test_bad_arguments(void)
{
    rh_key_t k = make_key(1);
    assert(rh_keystore_core_load(0, &k) == -1);
    assert(rh_keystore_core_load(&backend, 0) == -1);
    assert(rh_keystore_core_store(0, &k) == -1);
    assert(rh_keystore_core_store(&backend, 0) == -1);
}

int main(void)
{
    test_crc();
    test_empty();
    test_alternates();
    test_power_loss_mid_write();
    test_corrupt_slot_ignored();
    test_sequence_wrap();
    test_scope_is_covered_by_crc();
    test_bad_arguments();
    printf("keystore core: ok\n");
    return 0;
}
