# ---------------------------------------------------------------------------
#  Kconfig Integration for RoboHub Common
# ---------------------------------------------------------------------------

# Path to the Python generator
find_package(Python3 COMPONENTS Interpreter REQUIRED)
set(KCONFIG_GEN ${CMAKE_CURRENT_LIST_DIR}/../tools/kconfig/kconfig_gen.py)

# All Kconfig fragments for HAL interfaces
set(KCONFIG_INPUTS
    ${CMAKE_CURRENT_LIST_DIR}/../Kconfig
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.gpio
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.i2c
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.spi
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.uart
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.rs485
    ${CMAKE_CURRENT_LIST_DIR}/../kconfig/hal/Kconfig.timer
)

set(ROBOHUB_KCONFIG_FILE "" CACHE FILEPATH
    "Application Kconfig override file, such as project.conf")

# Output header
set(KCONFIG_OUTPUT ${CMAKE_BINARY_DIR}/generated/kconfig.h)

set(KCONFIG_ARGS ${KCONFIG_INPUTS} ${KCONFIG_OUTPUT})
set(KCONFIG_CONFIG_DEPENDS)
if(ROBOHUB_KCONFIG_FILE)
    list(PREPEND KCONFIG_ARGS --config ${ROBOHUB_KCONFIG_FILE})
    list(APPEND KCONFIG_CONFIG_DEPENDS ${ROBOHUB_KCONFIG_FILE})
endif()

# Ensure output directory exists
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/generated)

# Generate kconfig.h from all Kconfig fragments
add_custom_command(
    OUTPUT ${KCONFIG_OUTPUT}
    COMMAND ${Python3_EXECUTABLE} ${KCONFIG_GEN} ${KCONFIG_ARGS}
    DEPENDS ${KCONFIG_INPUTS} ${KCONFIG_GEN} ${KCONFIG_CONFIG_DEPENDS}
    COMMENT "Generating kconfig.h for RoboHub Common"
)

# Build target
add_custom_target(kconfig_common ALL DEPENDS ${KCONFIG_OUTPUT})

# Export header path
set(KCONFIG_HEADER ${KCONFIG_OUTPUT})
