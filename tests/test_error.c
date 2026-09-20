#include "error.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    assert(strcmp(robohub_error_to_string(ROBOHUB_OK), "OK") == 0);
    assert(strcmp(robohub_error_to_string(ROBOHUB_ERR_TIMEOUT),
                  "Operation timed out") == 0);
    assert(strcmp(robohub_error_to_string((RoboHubError)-999),
                  "Unknown error") == 0);
    return 0;
}
