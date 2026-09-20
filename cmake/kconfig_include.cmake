# ---------------------------------------------------------------------------
#  Kconfig Include Helper for RoboHub Common
# ---------------------------------------------------------------------------

# This file is included by other modules that depend on RoboHub Common.
# It exposes the generated kconfig.h so dependent modules can include it.

if(NOT DEFINED KCONFIG_HEADER)
    message(FATAL_ERROR "KCONFIG_HEADER is not defined. Did you include Kconfig.cmake?")
endif()

# Export include directory for generated Kconfig header
include_directories(${CMAKE_BINARY_DIR}/generated)

# Optional: expose variable to parent modules
set(ROBOHUB_COMMON_KCONFIG ${KCONFIG_HEADER} PARENT_SCOPE)
