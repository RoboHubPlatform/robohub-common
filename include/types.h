/**
 * @file type.h
 * @brief Generic data types for RoboHub Platform.
 *
 * Defines platform-independent types used across HAL, drivers, registry,
 * platform manager and all RoboHub modules.
 *
 * @project RoboHub Platform
 * @author Aurel
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Basic integer types */
typedef uint8_t   rh_u8;
typedef uint16_t  rh_u16;
typedef uint32_t  rh_u32;
typedef uint64_t  rh_u64;

typedef int8_t    rh_i8;
typedef int16_t   rh_i16;
typedef int32_t   rh_i32;
typedef int64_t   rh_i64;

/* Floating point */
typedef float     rh_f32;
typedef double    rh_f64;

/* Boolean */
typedef enum {
    RH_FALSE = 0,
    RH_TRUE  = 1
} rh_bool;

/* Size and index */
typedef rh_u32 rh_size;
typedef rh_u32 rh_index;

/* Time */
typedef rh_u64 rh_millis;
typedef rh_u64 rh_micros;

/* Identifiers */
typedef rh_u32  rh_id;
typedef const char* rh_name;

/* Callback */
typedef void (*rh_callback)(void* user_data);

/* Buffer */
typedef struct {
    rh_u8* data;
    rh_size size;
} rh_buffer;

/* String view */
typedef struct {
    const char* ptr;
    rh_size len;
} rh_strview;

/* Version */
typedef struct {
    rh_u16 major;
    rh_u16 minor;
    rh_u16 patch;
} rh_version;

#ifdef __cplusplus
}
#endif
