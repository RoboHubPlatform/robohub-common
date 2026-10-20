#include "registry.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    RoboHubGPIO gpio = {0};
    char mutable_name[] = "GPIO_MAIN";

    robohub_registry_init();

    /* Nothing is registered, so every kind is empty — the question a smoke asks
     * before a platform has brought anything up. */
    assert(robohub_count_gpio() == 0);
    assert(robohub_count_i2c() == 0);
    assert(robohub_count_timer() == 0);

    assert(robohub_get_gpio("missing") == NULL);
    assert(robohub_register_gpio(NULL, &gpio) == ROBOHUB_ERR_INVALID_ARG);
    assert(robohub_register_gpio(mutable_name, &gpio) == ROBOHUB_OK);

    /* A registered instance is counted, and only its own kind. */
    assert(robohub_count_gpio() == 1);
    assert(robohub_count_i2c() == 0);

    mutable_name[0] = 'X';
    assert(robohub_get_gpio("GPIO_MAIN") == &gpio);
    assert(robohub_register_gpio("GPIO_MAIN", &gpio) == ROBOHUB_ERR_BUSY);

    robohub_registry_init();
    assert(robohub_get_gpio("GPIO_MAIN") == NULL);

    assert(robohub_register_gpio("GPIO_MAIN", &gpio) == ROBOHUB_OK);
    robohub_registry_deinit();
    assert(robohub_get_gpio("GPIO_MAIN") == NULL);

    /* An analog converter and a PWM peripheral are their own kinds, each
     * registered and found by name like every other instance. */
    RoboHubADC adc = {0};
    RoboHubPWM pwm = {0};
    assert(robohub_count_adc() == 0);
    assert(robohub_count_pwm() == 0);
    assert(robohub_register_adc("adc0", &adc) == ROBOHUB_OK);
    assert(robohub_register_pwm("pwm0", &pwm) == ROBOHUB_OK);
    assert(robohub_get_adc("adc0") == &adc);
    assert(robohub_get_pwm("pwm0") == &pwm);
    assert(robohub_count_adc() == 1);
    assert(robohub_count_pwm() == 1);
    /* Registering a name twice in the same kind is refused, as for any other. */
    assert(robohub_register_adc("adc0", &adc) == ROBOHUB_ERR_BUSY);
    assert(robohub_get_adc("pwm0") == NULL);

    return 0;
}
