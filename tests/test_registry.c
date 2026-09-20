#include "registry.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    RoboHubGPIO gpio = {0};
    char mutable_name[] = "GPIO_MAIN";

    robohub_registry_init();

    assert(robohub_get_gpio("missing") == NULL);
    assert(robohub_register_gpio(NULL, &gpio) == ROBOHUB_ERR_INVALID_ARG);
    assert(robohub_register_gpio(mutable_name, &gpio) == ROBOHUB_OK);

    mutable_name[0] = 'X';
    assert(robohub_get_gpio("GPIO_MAIN") == &gpio);
    assert(robohub_register_gpio("GPIO_MAIN", &gpio) == ROBOHUB_ERR_BUSY);

    robohub_registry_init();
    assert(robohub_get_gpio("GPIO_MAIN") == NULL);

    assert(robohub_register_gpio("GPIO_MAIN", &gpio) == ROBOHUB_OK);
    robohub_registry_deinit();
    assert(robohub_get_gpio("GPIO_MAIN") == NULL);

    return 0;
}
