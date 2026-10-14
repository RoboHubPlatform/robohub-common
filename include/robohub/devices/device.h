/**
 * @file device.h
 * @brief Device identity for the RoboHub Platform.
 *
 * A device is not a chip and not a board. It is a thing the ecosystem talks
 * about by name — an inverter, a meter, a sensor — that a gateway lists,
 * attributes and addresses. The chip-level `drivers/` interfaces describe how a
 * part is read; this describes what a reading is *of*, so a power figure from a
 * Growatt inverter and one from a Hoymiles microinverter are the same kind of
 * statement and can be listed together.
 *
 * A device is named the way the gateway names it: the area it belongs to and the
 * node within that area. `node_id` is what a board or an integration sets;
 * `area_id` is the subtree it sits in. The gateway keys its state by the pair, so
 * two devices with the same node id in different areas are two devices, which is
 * what lets a tree of gateways attribute a reading without a source table.
 *
 * The values here are the wire's. The schema's `DeviceId` and `DeviceKind` are
 * the same numbers, so a `RoboHubPowerReading` maps onto a `PowerReading`
 * payload field for field and neither end has to be told which is which.
 *
 * @project RoboHub Platform
 * @author Aurel Miron
 * @version 0.1
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef INCLUDE_ROBOHUB_DEVICES_DEVICE_H
#define INCLUDE_ROBOHUB_DEVICES_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma once

/* -------------------------------------------------------------------------- */
/*  Includes                                                                  */
/* -------------------------------------------------------------------------- */
#include "types.h"

/* -------------------------------------------------------------------------- */
/*  Identity                                                                  */
/* -------------------------------------------------------------------------- */

/** The area a device is in; comparable to the schema's `area_id`. */
#define ROBOHUB_AREA_UNASSIGNED 0u

/**
 * @brief A device, unique across an area: where it is and which node it is.
 *
 * The pair is the key a gateway stores a device under, so a device is addressed
 * the same way whether it is wired to the gateway or is behind a relay.
 */
typedef struct {
    /** The area — the subtree a gateway owns. */
    rh_u16 area_id;
    /** The node within that area. */
    rh_u16 node_id;
} RoboHubDeviceId;

/**
 * @brief What kind of device something is.
 *
 * The numbers are the schema's `DeviceKind`, so a kind crosses the wire
 * unchanged. The kind is what a gateway or a client reads to know how to
 * interpret a record — an inverter's fields are power and energy, a meter's are
 * the same — without being told the part number.
 */
typedef enum {
    ROBOHUB_DEVICE_UNKNOWN  = 0,
    ROBOHUB_DEVICE_INVERTER = 1,
    ROBOHUB_DEVICE_METER    = 2,
    ROBOHUB_DEVICE_SENSOR   = 3,
    ROBOHUB_DEVICE_GATEWAY  = 4,
    ROBOHUB_DEVICE_BATTERY  = 5
} RoboHubDeviceKind;

/* -------------------------------------------------------------------------- */
/*  Functions                                                                 */
/* -------------------------------------------------------------------------- */

/** A stable lower-case word for a kind, for logs and JSON. Never null. */
const char* robohub_device_kind_name(RoboHubDeviceKind kind);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ROBOHUB_DEVICES_DEVICE_H */
