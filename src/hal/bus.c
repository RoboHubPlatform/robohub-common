/*
 * bus.c — the common register bus over I2C and SPI.
 *
 * The two backends are the only code in the platform that knows the wire. The
 * I2C one turns a register access into a write followed by a read, or a single
 * write of register-and-data; the SPI one sets the command bit and holds the
 * chip select for the length of the transfer. Above them, the `_open` calls are
 * the registry layer: they look a controller up by the name a board registered
 * it under and hand back a bound bus, so neither a board nor a driver ever names
 * a peripheral's struct.
 *
 * @project RoboHub Platform
 * @license MIT
 */

#include "robohub/hal/bus.h"
#include "registry.h"

#include <stddef.h>
#include <string.h>

/* The read bit an SPI part's command byte sets. Most parts use bit 7. */
#define ROBOHUB_BUS_SPI_READ_FLAG 0x80u

/* -------------------------------------------------------------------------- */
/*  I2C backend                                                               */
/* -------------------------------------------------------------------------- */

/* A register write is the register number followed by the bytes, in one
 * transaction: splitting it would have the part read the first data byte as a
 * new register number, which is a wrong write that looks like a right one. */
static RoboHubError bus_i2c_write_reg(RoboHubBus* self, rh_u8 reg, const rh_u8* data, rh_size len)
{
    RoboHubBusI2C* ctx = (RoboHubBusI2C*)self;
    if (ctx->i2c == NULL || ctx->i2c->write == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    if (len > ROBOHUB_BUS_MAX_XFER) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    if (len > 0u && data == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    rh_u8 buffer[ROBOHUB_BUS_MAX_XFER + 1u];
    buffer[0] = reg;
    if (len > 0u) {
        memcpy(&buffer[1], data, len);
    }
    return ctx->i2c->write(ctx->i2c, ctx->addr, buffer, len + 1u);
}

/* A register read sets the pointer with a one-byte write and then reads. The
 * HAL has no combined write-read, so the two are separate transactions; a part
 * that needs a repeated start is one a platform's controller has to provide
 * through its own read, which is why the pointer write is delegated rather than
 * inlined here. */
static RoboHubError bus_i2c_read_reg(RoboHubBus* self, rh_u8 reg, rh_u8* data, rh_size len)
{
    RoboHubBusI2C* ctx = (RoboHubBusI2C*)self;
    if (ctx->i2c == NULL || ctx->i2c->write == NULL || ctx->i2c->read == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    if (data == NULL && len > 0u) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    RoboHubError err = ctx->i2c->write(ctx->i2c, ctx->addr, &reg, 1u);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    return ctx->i2c->read(ctx->i2c, ctx->addr, data, len);
}

static RoboHubError bus_i2c_write(RoboHubBus* self, const rh_u8* data, rh_size len)
{
    RoboHubBusI2C* ctx = (RoboHubBusI2C*)self;
    if (ctx->i2c == NULL || ctx->i2c->write == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    return ctx->i2c->write(ctx->i2c, ctx->addr, data, len);
}

static RoboHubError bus_i2c_read(RoboHubBus* self, rh_u8* data, rh_size len)
{
    RoboHubBusI2C* ctx = (RoboHubBusI2C*)self;
    if (ctx->i2c == NULL || ctx->i2c->read == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    return ctx->i2c->read(ctx->i2c, ctx->addr, data, len);
}

RoboHubError robohub_bus_i2c(RoboHubBusI2C* self,
                             rh_name name,
                             RoboHubI2C* i2c,
                             rh_u8 addr,
                             rh_u32 speed_hz)
{
    if (self == NULL || i2c == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    memset(self, 0, sizeof(*self));
    self->i2c = i2c;
    self->addr = addr;

    self->bus.name = name;
    self->bus.context = self;
    self->bus.write_reg = bus_i2c_write_reg;
    self->bus.read_reg = bus_i2c_read_reg;
    self->bus.write = bus_i2c_write;
    self->bus.read = bus_i2c_read;

    if (speed_hz != 0u && i2c->init != NULL) {
        return i2c->init(i2c, speed_hz);
    }
    return ROBOHUB_OK;
}

/* -------------------------------------------------------------------------- */
/*  SPI backend                                                               */
/* -------------------------------------------------------------------------- */

static RoboHubError bus_spi_cs(RoboHubBusSPI* ctx, rh_u32 level)
{
    if (ctx->cs != NULL && ctx->cs->write != NULL) {
        return ctx->cs->write(ctx->cs, ctx->cs_pin, level);
    }
    return ROBOHUB_OK;
}

/* A register write is the register number with the read bit clear, then the
 * bytes, under one chip select. */
static RoboHubError bus_spi_write_reg(RoboHubBus* self, rh_u8 reg, const rh_u8* data, rh_size len)
{
    RoboHubBusSPI* ctx = (RoboHubBusSPI*)self;
    if (ctx->spi == NULL || ctx->spi->transfer == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    if (len > ROBOHUB_BUS_MAX_XFER) {
        return ROBOHUB_ERR_INVALID_ARG;
    }
    if (len > 0u && data == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    rh_u8 read_flag = ctx->read_flag != 0u ? ctx->read_flag : (rh_u8)ROBOHUB_BUS_SPI_READ_FLAG;
    rh_u8 buffer[ROBOHUB_BUS_MAX_XFER + 1u];
    buffer[0] = (rh_u8)(reg & (rh_u8)~read_flag);
    if (len > 0u) {
        memcpy(&buffer[1], data, len);
    }

    RoboHubError err = bus_spi_cs(ctx, 0u);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    err = ctx->spi->transfer(ctx->spi, buffer, NULL, len + 1u);
    (void)bus_spi_cs(ctx, 1u);
    return err;
}

/* A register read is the register number with the read bit set, one byte out,
 * then the answer clocked in. */
static RoboHubError bus_spi_read_reg(RoboHubBus* self, rh_u8 reg, rh_u8* data, rh_size len)
{
    RoboHubBusSPI* ctx = (RoboHubBusSPI*)self;
    if (ctx->spi == NULL || ctx->spi->transfer == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }
    if (data == NULL && len > 0u) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    rh_u8 read_flag = ctx->read_flag != 0u ? ctx->read_flag : (rh_u8)ROBOHUB_BUS_SPI_READ_FLAG;
    rh_u8 command = (rh_u8)(reg | read_flag);

    RoboHubError err = bus_spi_cs(ctx, 0u);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    err = ctx->spi->transfer(ctx->spi, &command, NULL, 1u);
    if (ROBOHUB_SUCCEEDED(err) && len > 0u) {
        err = ctx->spi->transfer(ctx->spi, NULL, data, len);
    }
    (void)bus_spi_cs(ctx, 1u);
    return err;
}

static RoboHubError bus_spi_write(RoboHubBus* self, const rh_u8* data, rh_size len)
{
    RoboHubBusSPI* ctx = (RoboHubBusSPI*)self;
    if (ctx->spi == NULL || ctx->spi->transfer == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }

    RoboHubError err = bus_spi_cs(ctx, 0u);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    err = ctx->spi->transfer(ctx->spi, data, NULL, len);
    (void)bus_spi_cs(ctx, 1u);
    return err;
}

static RoboHubError bus_spi_read(RoboHubBus* self, rh_u8* data, rh_size len)
{
    RoboHubBusSPI* ctx = (RoboHubBusSPI*)self;
    if (ctx->spi == NULL || ctx->spi->transfer == NULL) {
        return ROBOHUB_ERR_NOT_SUPPORTED;
    }

    RoboHubError err = bus_spi_cs(ctx, 0u);
    if (ROBOHUB_FAILED(err)) {
        return err;
    }
    err = ctx->spi->transfer(ctx->spi, NULL, data, len);
    (void)bus_spi_cs(ctx, 1u);
    return err;
}

RoboHubError robohub_bus_spi(RoboHubBusSPI* self,
                             rh_name name,
                             RoboHubSPI* spi,
                             RoboHubGPIO* cs,
                             rh_u32 cs_pin,
                             rh_u8 read_flag,
                             rh_u32 speed_hz)
{
    if (self == NULL || spi == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    memset(self, 0, sizeof(*self));
    self->spi = spi;
    self->cs = cs;
    self->cs_pin = cs_pin;
    self->read_flag = read_flag != 0u ? read_flag : (rh_u8)ROBOHUB_BUS_SPI_READ_FLAG;

    self->bus.name = name;
    self->bus.context = self;
    self->bus.write_reg = bus_spi_write_reg;
    self->bus.read_reg = bus_spi_read_reg;
    self->bus.write = bus_spi_write;
    self->bus.read = bus_spi_read;

    if (speed_hz != 0u && spi->init != NULL) {
        return spi->init(spi, speed_hz);
    }
    return ROBOHUB_OK;
}

/* -------------------------------------------------------------------------- */
/*  Registry-specific binding                                                 */
/* -------------------------------------------------------------------------- */

RoboHubError robohub_bus_i2c_open(RoboHubBusI2C* self,
                                  rh_name controller,
                                  rh_u8 addr,
                                  rh_u32 speed_hz)
{
    if (self == NULL || controller == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    RoboHubI2C* i2c = robohub_get_i2c(controller);
    if (i2c == NULL) {
        return ROBOHUB_ERR_NOT_FOUND;
    }
    return robohub_bus_i2c(self, controller, i2c, addr, speed_hz);
}

RoboHubError robohub_bus_spi_open(RoboHubBusSPI* self,
                                  rh_name controller,
                                  RoboHubGPIO* cs,
                                  rh_u32 cs_pin,
                                  rh_u8 read_flag,
                                  rh_u32 speed_hz)
{
    if (self == NULL || controller == NULL) {
        return ROBOHUB_ERR_INVALID_ARG;
    }

    RoboHubSPI* spi = robohub_get_spi(controller);
    if (spi == NULL) {
        return ROBOHUB_ERR_NOT_FOUND;
    }
    return robohub_bus_spi(self, controller, spi, cs, cs_pin, read_flag, speed_hz);
}
