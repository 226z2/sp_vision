cmake_minimum_required(VERSION 3.20)

enable_language(C)

# ------------------------------------------------------------------------------
# Driver library (framework-agnostic)
# ------------------------------------------------------------------------------
# This module may be included as a subdirectory from a larger project.
# Avoid relying on CMAKE_SOURCE_DIR and compute our own root.
get_filename_component(COMMUNICATION_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

add_library(communication_drivers STATIC
    drivers/dm_02/dm_02.cpp
    drivers/dm_02/platform_posix.cpp

    ${COMMUNICATION_ROOT}/3rdparty/Communication/core/CRC8_CRC16.c
    ${COMMUNICATION_ROOT}/3rdparty/Communication/core/comm.c
    ${COMMUNICATION_ROOT}/3rdparty/Communication/core/uproto.c
)
add_library(Holon::communication::drivers ALIAS communication_drivers)

# Keep this library usable from C++17 projects (it doesn't require C++20).
target_compile_features(communication_drivers PUBLIC cxx_std_17)
set_target_properties(communication_drivers PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
)
target_include_directories(communication_drivers PUBLIC
    ${COMMUNICATION_ROOT}
)
