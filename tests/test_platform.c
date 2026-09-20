#include "platform.h"
#include "registry.h"

#include <assert.h>
#include <stddef.h>

static RoboHubGPIO gpio;

RoboHubError robohub_board_register(void)
{
    assert(robohub_register_gpio("GPIO_MAIN", &gpio) == ROBOHUB_OK);
    return ROBOHUB_ERR_FAIL;
}

int main(void)
{
    assert(robohub_platform_init() == ROBOHUB_ERR_FAIL);
    assert(robohub_get_gpio("GPIO_MAIN") == NULL);
    return 0;
}
