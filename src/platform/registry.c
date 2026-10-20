/**
 * @file registry.c
 * @brief HAL instance registry implementation for RoboHub Platform.
 *
 * Stores and retrieves HAL instances by name. Instances contain their own
 * platform-specific context, which is opaque to the registry.
 *
 * @project RoboHub Platform
 * @author Aurel
 * @version 1.0
 * @date 2026
 *
 * SPDX-License-Identifier: MIT
 */

#include "registry.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/*  Generic Registry Structure                                                 */
/* -------------------------------------------------------------------------- */

typedef struct {
    char*   name;
    void*   instance;
} RegistryEntry;

typedef struct {
    RegistryEntry* entries;
    rh_size        count;
    rh_size        capacity;
} Registry;

/* -------------------------------------------------------------------------- */
/*  Registry Helpers                                                           */
/* -------------------------------------------------------------------------- */

static void registry_init_single(Registry* reg)
{
    for (rh_size i = 0; i < reg->count; i++)
        free(reg->entries[i].name);

    free(reg->entries);
    reg->entries  = NULL;
    reg->count    = 0;
    reg->capacity = 0;
}

static RoboHubError registry_add(Registry* reg, rh_name name, void* instance)
{
    if (!name || !instance)
        return ROBOHUB_ERR_INVALID_ARG;

    for (rh_size i = 0; i < reg->count; i++) {
        if (strcmp(reg->entries[i].name, name) == 0)
            return ROBOHUB_ERR_BUSY;
    }

    if (reg->count == reg->capacity) {
        rh_size new_cap = (reg->capacity == 0) ? 4 : reg->capacity;
        if (reg->capacity != 0 && reg->capacity > UINT32_MAX / 2)
            return ROBOHUB_ERR_NO_MEM;
        if (reg->capacity != 0)
            new_cap *= 2;
    #if SIZE_MAX < UINT32_MAX
        if (new_cap > SIZE_MAX / sizeof(RegistryEntry))
            return ROBOHUB_ERR_NO_MEM;
    #endif

        void* new_mem = realloc(reg->entries,
                                (size_t)new_cap * sizeof(RegistryEntry));
        if (!new_mem)
            return ROBOHUB_ERR_NO_MEM;

        reg->entries  = (RegistryEntry*)new_mem;
        reg->capacity = new_cap;
    }

    size_t name_len = strlen(name);
    if (name_len == SIZE_MAX)
        return ROBOHUB_ERR_NO_MEM;

    char* owned_name = malloc(name_len + 1);
    if (!owned_name)
        return ROBOHUB_ERR_NO_MEM;
    memcpy(owned_name, name, name_len + 1);

    reg->entries[reg->count].name    = owned_name;
    reg->entries[reg->count].instance = instance;
    reg->count++;

    return ROBOHUB_OK;
}

static void* registry_get(Registry* reg, rh_name name)
{
    if (!name)
        return NULL;

    for (rh_size i = 0; i < reg->count; i++) {
        if (strcmp(reg->entries[i].name, name) == 0)
            return reg->entries[i].instance;
    }
    return NULL;
}

/* -------------------------------------------------------------------------- */
/*  Registries for each HAL subsystem                                          */
/* -------------------------------------------------------------------------- */

static Registry i2c_registry;
static Registry spi_registry;
static Registry uart_registry;
static Registry gpio_registry;
static Registry rs485_registry;
static Registry timer_registry;
static Registry onewire_registry;
static Registry dht_registry;
static Registry adc_registry;
static Registry pwm_registry;

/* -------------------------------------------------------------------------- */
/*  Initialization                                                             */
/* -------------------------------------------------------------------------- */

void robohub_registry_init(void)
{
    registry_init_single(&i2c_registry);
    registry_init_single(&spi_registry);
    registry_init_single(&uart_registry);
    registry_init_single(&gpio_registry);
    registry_init_single(&rs485_registry);
    registry_init_single(&timer_registry);
    registry_init_single(&onewire_registry);
    registry_init_single(&dht_registry);
    registry_init_single(&adc_registry);
    registry_init_single(&pwm_registry);
}

void robohub_registry_deinit(void)
{
    registry_init_single(&i2c_registry);
    registry_init_single(&spi_registry);
    registry_init_single(&uart_registry);
    registry_init_single(&gpio_registry);
    registry_init_single(&rs485_registry);
    registry_init_single(&timer_registry);
    registry_init_single(&onewire_registry);
    registry_init_single(&dht_registry);
    registry_init_single(&adc_registry);
    registry_init_single(&pwm_registry);
}

/* -------------------------------------------------------------------------- */
/*  Registration API                                                           */
/* -------------------------------------------------------------------------- */

RoboHubError robohub_register_i2c(rh_name name, RoboHubI2C* instance)
{
    return registry_add(&i2c_registry, name, instance);
}

RoboHubError robohub_register_spi(rh_name name, RoboHubSPI* instance)
{
    return registry_add(&spi_registry, name, instance);
}

RoboHubError robohub_register_uart(rh_name name, RoboHubUART* instance)
{
    return registry_add(&uart_registry, name, instance);
}

RoboHubError robohub_register_gpio(rh_name name, RoboHubGPIO* instance)
{
    return registry_add(&gpio_registry, name, instance);
}

RoboHubError robohub_register_rs485(rh_name name, RoboHubRS485* instance)
{
    return registry_add(&rs485_registry, name, instance);
}

RoboHubError robohub_register_timer(rh_name name, RoboHubTimer* instance)
{
    return registry_add(&timer_registry, name, instance);
}

RoboHubError robohub_register_onewire(rh_name name, RoboHubOneWire* instance)
{
    return registry_add(&onewire_registry, name, instance);
}

RoboHubError robohub_register_dht(rh_name name, RoboHubDHT* instance)
{
    return registry_add(&dht_registry, name, instance);
}

RoboHubError robohub_register_adc(rh_name name, RoboHubADC* instance)
{
    return registry_add(&adc_registry, name, instance);
}

RoboHubError robohub_register_pwm(rh_name name, RoboHubPWM* instance)
{
    return registry_add(&pwm_registry, name, instance);
}

/* -------------------------------------------------------------------------- */
/*  Lookup API                                                                 */
/* -------------------------------------------------------------------------- */

RoboHubI2C* robohub_get_i2c(rh_name name)
{
    return (RoboHubI2C*)registry_get(&i2c_registry, name);
}

RoboHubSPI* robohub_get_spi(rh_name name)
{
    return (RoboHubSPI*)registry_get(&spi_registry, name);
}

RoboHubUART* robohub_get_uart(rh_name name)
{
    return (RoboHubUART*)registry_get(&uart_registry, name);
}

RoboHubGPIO* robohub_get_gpio(rh_name name)
{
    return (RoboHubGPIO*)registry_get(&gpio_registry, name);
}

RoboHubRS485* robohub_get_rs485(rh_name name)
{
    return (RoboHubRS485*)registry_get(&rs485_registry, name);
}

RoboHubTimer* robohub_get_timer(rh_name name)
{
    return (RoboHubTimer*)registry_get(&timer_registry, name);
}

RoboHubOneWire* robohub_get_onewire(rh_name name)
{
    return (RoboHubOneWire*)registry_get(&onewire_registry, name);
}

RoboHubDHT* robohub_get_dht(rh_name name)
{
    return (RoboHubDHT*)registry_get(&dht_registry, name);
}

RoboHubADC* robohub_get_adc(rh_name name)
{
    return (RoboHubADC*)registry_get(&adc_registry, name);
}

RoboHubPWM* robohub_get_pwm(rh_name name)
{
    return (RoboHubPWM*)registry_get(&pwm_registry, name);
}

/* -------------------------------------------------------------------------- */
/*  Counts                                                                    */
/* -------------------------------------------------------------------------- */

/** How many instances one kind has. A board has one name per bus, so presence
 *  is the question a smoke can answer, and the name is not its business. */
static rh_size registry_count(const Registry* reg)
{
    return reg->count;
}

rh_size robohub_count_i2c(void)     { return registry_count(&i2c_registry); }
rh_size robohub_count_spi(void)     { return registry_count(&spi_registry); }
rh_size robohub_count_uart(void)    { return registry_count(&uart_registry); }
rh_size robohub_count_gpio(void)    { return registry_count(&gpio_registry); }
rh_size robohub_count_rs485(void)   { return registry_count(&rs485_registry); }
rh_size robohub_count_timer(void)   { return registry_count(&timer_registry); }
rh_size robohub_count_onewire(void) { return registry_count(&onewire_registry); }
rh_size robohub_count_dht(void)     { return registry_count(&dht_registry); }
rh_size robohub_count_adc(void)     { return registry_count(&adc_registry); }
rh_size robohub_count_pwm(void)     { return registry_count(&pwm_registry); }
