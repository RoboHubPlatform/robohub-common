/*
 * device.c — device identity helpers.
 *
 * The kind a device is crosses the wire as a number, and this is the one place
 * that number becomes a word. A log line, a JSON record and a person's terminal
 * all want the same word, so it is written once.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/devices/device.h"

const char* robohub_device_kind_name(RoboHubDeviceKind kind)
{
    switch (kind) {
    case ROBOHUB_DEVICE_INVERTER: return "inverter";
    case ROBOHUB_DEVICE_METER:    return "meter";
    case ROBOHUB_DEVICE_SENSOR:   return "sensor";
    case ROBOHUB_DEVICE_GATEWAY:  return "gateway";
    case ROBOHUB_DEVICE_BATTERY:  return "battery";
    case ROBOHUB_DEVICE_SWITCH:   return "switch";
    case ROBOHUB_DEVICE_ACTUATOR: return "actuator";
    case ROBOHUB_DEVICE_UNKNOWN:
    default:                      return "unknown";
    }
}
