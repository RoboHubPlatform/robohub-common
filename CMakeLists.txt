cmake_minimum_required(VERSION 3.16)

project(robohub-common LANGUAGES C VERSION 1.0.0)

include(CTest)

# ---------------------------------------------------------------------------
#  Configuration
# ---------------------------------------------------------------------------
#
# Nothing is merged here. Configuration is resolved by rho-kconfig and written
# beside the sources that use it, and a build that wants it asks for it.
#
# It used to be a Python generator run from this file, producing a `kconfig.h`
# that no source in this library included: the modules it declared belonged to a
# board, and a library that implements interfaces has no business deciding what a
# board has switched on.

# ---------------------------------------------------------------------------
#  Source directories
# ---------------------------------------------------------------------------

set(ROBOHUB_COMMON_SOURCES
    src/error.c

    src/hal/bus.c

    src/platform/platform.c
    src/platform/registry.c
    src/platform/net.c
)

# ---------------------------------------------------------------------------
#  Build library
# ---------------------------------------------------------------------------

add_library(robohub-common STATIC ${ROBOHUB_COMMON_SOURCES})

target_include_directories(robohub-common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robohub
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robohub/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robohub/platform
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robohub/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robohub/devices
    $<INSTALL_INTERFACE:include>
)

target_compile_features(robohub-common PUBLIC c_std_11)

# These run on the machine doing the building, so they are only registered for a
# hosted build. A bare-metal build links a C library that has no `_kill` or
# `_getpid` for a host test to resolve against, and a test executable built there
# reaches the link step looking for a C runtime startup rather than the board's —
# which is reported as a missing `Reset_Handler`, a symbol this module has nothing
# to do with.
#
# The flags come from rho-target, which read the `testing:` section of this
# module's manifest. Building this project on its own with plain cmake defines
# neither, and the tests are then not registered: a test is something a build is
# told to run, not something every build inherits.
if(RHO_BUILD_HOST_TESTS AND NOT RHO_TARGET_IS_BAREMETAL)
    message(STATUS "[robohub-common] registering host unit tests")
    enable_testing()

    add_executable(robohub-test-error tests/test_error.c)
    target_link_libraries(robohub-test-error PRIVATE robohub-common)
    add_test(NAME robohub-test-error COMMAND robohub-test-error)

    add_executable(robohub-test-registry tests/test_registry.c)
    target_link_libraries(robohub-test-registry PRIVATE robohub-common)
    add_test(NAME robohub-test-registry COMMAND robohub-test-registry)

    add_executable(robohub-test-platform tests/test_platform.c)
    target_link_libraries(robohub-test-platform PRIVATE robohub-common)
    add_test(NAME robohub-test-platform COMMAND robohub-test-platform)

    add_executable(robohub-test-net-defaults tests/test_net_defaults.c)
    target_link_libraries(robohub-test-net-defaults PRIVATE robohub-common)
    add_test(NAME robohub-test-net-defaults COMMAND robohub-test-net-defaults)

    # Implements the contract over POSIX in the test itself, so it overrides the
    # weak defaults the way a platform does and exercises a real loopback socket.
    add_executable(robohub-test-net-posix tests/test_net_posix.c)
    target_link_libraries(robohub-test-net-posix PRIVATE robohub-common)
    add_test(NAME robohub-test-net-posix COMMAND robohub-test-net-posix)
endif()

# ---------------------------------------------------------------------------
#  Dependencies
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
#  Export module metadata (module.yml)
# ---------------------------------------------------------------------------

install(FILES module.yml DESTINATION share/robohub-common)

# ---------------------------------------------------------------------------
#  Export headers
# ---------------------------------------------------------------------------

install(DIRECTORY include/ DESTINATION include)

# ---------------------------------------------------------------------------
#  Export library
# ---------------------------------------------------------------------------

install(TARGETS robohub-common
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
