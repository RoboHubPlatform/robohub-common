cmake_minimum_required(VERSION 3.16)

project(robohub-common LANGUAGES C VERSION 1.0.0)

include(CTest)

# ---------------------------------------------------------------------------
#  Load Kconfig (generate kconfig.h)
# ---------------------------------------------------------------------------

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Kconfig.cmake)

# Make generated kconfig.h visible to all sources
include_directories(${CMAKE_BINARY_DIR}/generated)

# ---------------------------------------------------------------------------
#  Source directories
# ---------------------------------------------------------------------------

set(ROBOHUB_COMMON_SOURCES
    src/error.c

    src/platform/platform.c
    src/platform/registry.c
)

# ---------------------------------------------------------------------------
#  Build library
# ---------------------------------------------------------------------------

add_library(robohub-common STATIC ${ROBOHUB_COMMON_SOURCES})

add_dependencies(robohub-common kconfig_common)

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

if(BUILD_TESTING)
    add_executable(robohub-test-error tests/test_error.c)
    target_link_libraries(robohub-test-error PRIVATE robohub-common)
    add_test(NAME robohub-test-error COMMAND robohub-test-error)

    add_executable(robohub-test-registry tests/test_registry.c)
    target_link_libraries(robohub-test-registry PRIVATE robohub-common)
    add_test(NAME robohub-test-registry COMMAND robohub-test-registry)

    add_executable(robohub-test-platform tests/test_platform.c)
    target_link_libraries(robohub-test-platform PRIVATE robohub-common)
    add_test(NAME robohub-test-platform COMMAND robohub-test-platform)
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
