/*
 * rs485_uart.c — RS485 over a UART and a transceiver direction pin.
 *
 * The UART carries the bytes and the pin chooses the direction; this is the
 * joining of the two. It is the same idea as the common register bus: one
 * contract over whatever the platform happened to implement underneath, so a
 * protocol above is not written for a transceiver.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/hal/rs485_uart.h"

#include <stddef.h>

/* Bits in one character on the line at 8N1: start, eight data, stop. Used to
 * turn a baud rate into the time the last character takes to leave. */
#define RS485_BITS_PER_CHARACTER 10u
/* How many character times to wait before releasing the line. Two is the
 * datasheet's own advice for a transceiver's enable-to-drive skew and leaves
 * margin for a UART that empties a byte late. */
#define RS485_CHARACTER_MARGIN 2u
/* How often the timed read looks for more bytes while it waits. */
#define RS485_POLL_US 200u

static RoboHubRS485Uart* bus(RoboHubRS485* self)
{
    return (RoboHubRS485Uart*)self->context;
}

/* -------------------------------------------------------------------------- */
/*  Direction                                                                 */
/* -------------------------------------------------------------------------- */

static RoboHubError rs485_set_direction(RoboHubRS485* self, rh_u32 dir)
{
    RoboHubRS485Uart* ctx = bus(self);
    if (ctx->de == NULL || ctx->de->write == NULL) {
        /* Automatic direction: the transceiver decides, and the call is the
         * contract's and not the hardware's. */
        return ROBOHUB_OK;
    }
    rh_u32 level;
    if (dir == ROBOHUB_RS485_TX) {
        level = ctx->de_active_high ? 1u : 0u;
    } else {
        level = ctx->de_active_high ? 0u : 1u;
    }
    return ctx->de->write(ctx->de, ctx->de_pin, level);
}

/* -------------------------------------------------------------------------- */
/*  Bring-up                                                                  */
/* -------------------------------------------------------------------------- */

static RoboHubError rs485_init(RoboHubRS485* self, rh_u32 baudrate)
{
    RoboHubRS485Uart* ctx = bus(self);
    if (ctx->uart == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    if (ctx->de != NULL && ctx->de->set_mode != NULL) {
        RoboHubError err = ctx->de->set_mode(ctx->de, ctx->de_pin, ROBOHUB_GPIO_MODE_OUTPUT);
        if (ROBOHUB_FAILED(err)) {
            return err;
        }
    }

    if (ctx->uart->init != NULL) {
        RoboHubError err = ctx->uart->init(ctx->uart, baudrate);
        if (ROBOHUB_FAILED(err)) {
            return err;
        }
    }
    ctx->baudrate = baudrate;

    /* Rest the line in receive, so the first response is not missed. */
    return rs485_set_direction(self, ROBOHUB_RS485_RX);
}

/* -------------------------------------------------------------------------- */
/*  Transmit                                                                  */
/* -------------------------------------------------------------------------- */

/* How long the last character takes, with margin, in microseconds. */
static rh_u32 rs485_drain_us(const RoboHubRS485Uart* ctx)
{
    if (ctx->baudrate == 0u) {
        return 0u;
    }
    rh_u32 bits = RS485_BITS_PER_CHARACTER * RS485_CHARACTER_MARGIN;
    return (rh_u32)(((rh_u64)bits * 1000000u) / ctx->baudrate);
}

static RoboHubError rs485_write(RoboHubRS485* self, const rh_u8* data, rh_size len)
{
    RoboHubRS485Uart* ctx = bus(self);
    if (ctx->uart == NULL || ctx->uart->write == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }

    RoboHubError err = rs485_set_direction(self, ROBOHUB_RS485_TX);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }

    err = ctx->uart->write(ctx->uart, data, len);

    /* Let the last character leave before releasing the line. Without a timer
     * there is nothing to measure the wait with, so the line is released at
     * once and a platform whose write already drained is no worse off. */
    if (ctx->timer != NULL && ctx->timer->delay_us != NULL) {
        rh_u32 wait_us = rs485_drain_us(ctx);
        if (wait_us != 0u) {
            ctx->timer->delay_us(ctx->timer, wait_us);
        }
    }

    RoboHubError restore = rs485_set_direction(self, ROBOHUB_RS485_RX);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    return restore;
}

/* -------------------------------------------------------------------------- */
/*  Receive                                                                   */
/* -------------------------------------------------------------------------- */

static RoboHubError rs485_read(RoboHubRS485* self, rh_u8* data, rh_size len)
{
    RoboHubRS485Uart* ctx = bus(self);
    if (ctx->uart == NULL || ctx->uart->read == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    return ctx->uart->read(ctx->uart, data, len);
}

/* The RTU inter-frame silence, in bit times: 3.5 characters at 8N1 is 35 bits.
 * It is what ends a frame — the answer is exactly as long as the device made it,
 * and a reply is done when the line has been quiet that long, not when a length
 * the master guessed has arrived. */
#define RS485_GAP_BITS 35u

/* Read as much as arrives within the window and return how much. A half-duplex
 * answer is exactly as long as the device made it, so a short read is a real
 * outcome and the count is the result. A frame ends at the inter-frame silence,
 * which keeps a reply from waiting out the whole window once it has arrived. */
static rh_size rs485_read_timed(RoboHubRS485* self, rh_u8* data, rh_size len, rh_u32 timeout_ms)
{
    RoboHubRS485Uart* ctx = bus(self);
    if (ctx->uart == NULL || ctx->uart->read_available == NULL || data == NULL) {
        return 0u;
    }

    rh_bool use_timer = (ctx->timer != NULL && ctx->timer->get_micros != NULL);
    rh_micros started = use_timer ? ctx->timer->get_micros(ctx->timer) : 0u;
    rh_micros last_byte = started;
    rh_micros gap_us = (ctx->baudrate != 0u)
        ? (rh_micros)(((rh_u64)RS485_GAP_BITS * 1000000u) / ctx->baudrate)
        : 3500u;

    rh_size got = 0u;
    rh_u32 idle = 0u;
    rh_u32 spins = 0u;
    while (got < len) {
        rh_size n = ctx->uart->read_available(ctx->uart, data + got, len - got);
        if (n > 0u) {
            got += n;
            idle = 0u;
            if (use_timer) {
                last_byte = ctx->timer->get_micros(ctx->timer);
            }
            if (got >= len) {
                break;
            }
        }

        if (use_timer) {
            rh_micros now = ctx->timer->get_micros(ctx->timer);
            if (got > 0u && (now - last_byte) >= gap_us) {
                break; /* the frame ended in silence */
            }
            if ((now - started) >= (rh_micros)timeout_ms * 1000u) {
                break; /* the window closed with no complete frame */
            }
            /* A hard bound as well, so a clock that does not advance cannot spin
             * here forever. */
            if (++spins > (timeout_ms * 1000u) / RS485_POLL_US + 8u) {
                break;
            }
            if (ctx->timer->delay_us != NULL) {
                ctx->timer->delay_us(ctx->timer, RS485_POLL_US);
            }
        } else {
            /* No clock to measure against: a run of polls with nothing new ends
             * the frame, and a bound on the polls ends the window. */
            if (got > 0u) {
                idle++;
                if (idle >= (RS485_GAP_BITS / 2u)) {
                    break;
                }
            }
            if (++spins > (timeout_ms * 1000u) / RS485_POLL_US) {
                break;
            }
        }
    }
    return got;
}

/* -------------------------------------------------------------------------- */
/*  Binding                                                                   */
/* -------------------------------------------------------------------------- */

RoboHubError robohub_rs485_uart(RoboHubRS485Uart* self,
                                rh_name name,
                                RoboHubUART* uart,
                                RoboHubGPIO* de,
                                rh_u32 de_pin,
                                rh_bool de_active_high,
                                RoboHubTimer* timer)
{
    if (self == NULL || uart == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    self->uart = uart;
    self->de = de;
    self->de_pin = de_pin;
    self->de_active_high = de_active_high;
    self->timer = timer;
    self->baudrate = 0u;

    self->interface.name = name;
    self->interface.context = self;
    self->interface.init = rs485_init;
    self->interface.write = rs485_write;
    self->interface.read = rs485_read;
    self->interface.read_timed = rs485_read_timed;
    self->interface.set_direction = rs485_set_direction;

    return ROBOHUB_OK;
}
