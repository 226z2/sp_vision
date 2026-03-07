cmake_minimum_required(VERSION 3.16)

add_library(tools OBJECT 
    exiter.cpp
    extended_kalman_filter.cpp
    ransac_sine_fitter.cpp
    img_tools.cpp
    math_tools.cpp
    plotter.cpp
    trajectory.cpp
    recorder.cpp
    logger.cpp
    pid.cpp
    crc.cpp
)

target_include_directories(tools PUBLIC ${PROJECT_SOURCE_DIR})

if(SPV_ENABLE_FOXGLOVE)
    add_dependencies(tools spv_foxglove_codegen)
    target_include_directories(tools PRIVATE
        "${SPV_FOXGLOVE_GENERATED_DIR}"
        "${PROJECT_SOURCE_DIR}/3rdparty/flatbuffers/include"
    )
    target_link_libraries(tools PUBLIC foxglove_websocket)
    target_compile_definitions(tools PUBLIC SPV_ENABLE_FOXGLOVE=1)
endif()

if(SPV_PLOTTER_BACKEND STREQUAL "foxglove")
    target_compile_definitions(tools PUBLIC SPV_PLOTTER_BACKEND_FOXGLOVE=1)
endif()

if(SPV_LOGGER_FOXGLOVE_SINK)
    target_compile_definitions(tools PUBLIC SPV_LOGGER_FOXGLOVE_SINK=1)
endif()

target_compile_definitions(tools PUBLIC SPV_FOXGLOVE_JPEG_QUALITY=${SPV_FOXGLOVE_JPEG_QUALITY})
