cmake_minimum_required(VERSION 3.16.3)
# 设置CMake策略以消除OpenVINO警告
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
project(sp_vision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
message(STATUS "--------------------CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}--------------------")

set(SPV_PLOTTER_BACKEND "udp" CACHE STRING "Plotter backend: udp|foxglove")
set_property(CACHE SPV_PLOTTER_BACKEND PROPERTY STRINGS udp foxglove)
option(SPV_ENABLE_FOXGLOVE "Enable Foxglove debug transport support" OFF)
option(SPV_LOGGER_FOXGLOVE_SINK "Mirror spdlog output to Foxglove.Log channel" OFF)
set(SPV_FOXGLOVE_JPEG_QUALITY "60" CACHE STRING "JPEG quality for Foxglove compressed image")
set(SPV_FLATC_PATH "${PROJECT_SOURCE_DIR}/tools/bin/flatc/linux-x86_64/flatc" CACHE FILEPATH "Path to flatc binary")
set(SPV_FOXGLOVE_GENERATED_DIR "${PROJECT_SOURCE_DIR}/generated/foxglove" CACHE PATH "Output directory for generated Foxglove FlatBuffers")

if(NOT SPV_PLOTTER_BACKEND STREQUAL "udp" AND NOT SPV_PLOTTER_BACKEND STREQUAL "foxglove")
    message(FATAL_ERROR "SPV_PLOTTER_BACKEND must be udp or foxglove, got: ${SPV_PLOTTER_BACKEND}")
endif()
if(SPV_PLOTTER_BACKEND STREQUAL "foxglove")
    set(SPV_ENABLE_FOXGLOVE ON)
endif()

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog CONFIG REQUIRED PATHS /opt/spdlog/lib/cmake/spdlog NO_DEFAULT_PATH)
find_package(Eigen3 REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
set(OpenVINO_DIR "/opt/intel/openvino_2024.6.0/runtime/cmake/")
find_package(OpenVINO REQUIRED)

include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR})

if(SPV_ENABLE_FOXGLOVE)
    if(NOT EXISTS "${SPV_FLATC_PATH}")
        message(FATAL_ERROR
            "SPV_ENABLE_FOXGLOVE=ON requires flatc at ${SPV_FLATC_PATH}. "
            "Run: bash tools/foxglove/download_flatc.sh")
    endif()
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/3rdparty/flatbuffers/include/flatbuffers/flatbuffers.h")
        message(FATAL_ERROR
            "SPV_ENABLE_FOXGLOVE=ON requires FlatBuffers headers at "
            "${PROJECT_SOURCE_DIR}/3rdparty/flatbuffers/include. "
            "Download and extract the FlatBuffers release include tree there.")
    endif()

    if(NOT WEBSOCKETPP_INCLUDE_DIR)
        set(WEBSOCKETPP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/websocketpp")
    endif()
    add_subdirectory(3rdparty/ws-protocol)

    file(GLOB SPV_FOXGLOVE_SCHEMA_FILES "${PROJECT_SOURCE_DIR}/3rdparty/ws-protocol/flatbuffers/*.fbs")
    add_custom_command(
        OUTPUT "${SPV_FOXGLOVE_GENERATED_DIR}/.stamp"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SPV_FOXGLOVE_GENERATED_DIR}"
        COMMAND /bin/bash "${PROJECT_SOURCE_DIR}/tools/foxglove/gen_schemas.sh"
            "${SPV_FLATC_PATH}"
            "${PROJECT_SOURCE_DIR}/3rdparty/ws-protocol/flatbuffers"
            "${SPV_FOXGLOVE_GENERATED_DIR}"
        COMMAND ${CMAKE_COMMAND} -E touch "${SPV_FOXGLOVE_GENERATED_DIR}/.stamp"
        DEPENDS
            "${PROJECT_SOURCE_DIR}/tools/foxglove/gen_schemas.sh"
            ${SPV_FOXGLOVE_SCHEMA_FILES}
        VERBATIM
    )
    add_custom_target(spv_foxglove_codegen DEPENDS "${SPV_FOXGLOVE_GENERATED_DIR}/.stamp")
endif()

set(SP_THIRD_PARTY_TARGETS
    fmt::fmt
    spdlog::spdlog
    yaml-cpp
    nlohmann_json::nlohmann_json)

add_subdirectory(tools)
add_subdirectory(io)
add_subdirectory(tasks/auto_aim)
add_subdirectory(tasks/omniperception)

####################src###################
add_executable(standard src/standard.cpp)
add_executable(mt_standard src/mt_standard.cpp)
add_executable(standard_mpc src/standard_mpc.cpp)
add_executable(auto_aim_debug_mpc src/auto_aim_debug_mpc.cpp)
add_executable(mt_auto_aim_debug src/mt_auto_aim_debug.cpp)
add_executable(uav src/uav.cpp)
add_executable(uav_debug src/uav_debug.cpp)

target_link_libraries(standard PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(mt_standard PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(standard_mpc PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(auto_aim_debug_mpc PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(mt_auto_aim_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(uav PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)
target_link_libraries(uav_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)

##################calibration################
add_executable(capture calibration/capture.cpp)
add_executable(calibrate_camera calibration/calibrate_camera.cpp)
add_executable(calibrate_handeye calibration/calibrate_handeye.cpp)
add_executable(calibrate_robotworld_handeye calibration/calibrate_robotworld_handeye.cpp)
add_executable(split_video calibration/split_video.cpp)

target_link_libraries(capture PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
target_link_libraries(calibrate_camera PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools)
target_link_libraries(calibrate_handeye PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools)
target_link_libraries(calibrate_robotworld_handeye PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools)
target_link_libraries(split_video PRIVATE ${OpenCV_LIBS} fmt::fmt tools)

##################tests##################
add_executable(auto_aim_test tests/auto_aim_test.cpp)
add_executable(camera_detect_test tests/camera_detect_test.cpp)
add_executable(camera_test tests/camera_test.cpp)
add_executable(camera_thread_test tests/camera_thread_test.cpp)
add_executable(cboard_test tests/cboard_test.cpp)
add_executable(fire_test tests/fire_test.cpp)
add_executable(detector_video_test tests/detector_video_test.cpp)
add_executable(gimbal_response_test tests/gimbal_response_test.cpp)
add_executable(multi_usbcamera_test tests/multi_usbcamera_test.cpp)
add_executable(usbcamera_detect_test tests/usbcamera_detect_test.cpp)
add_executable(usbcamera_test tests/usbcamera_test.cpp)
add_executable(handeye_test tests/handeye_test.cpp)
add_executable(dm_test tests/dm_test.cpp)
add_executable(dm_virtual_sender_test tests/dm_virtual_sender_test.cpp)
add_executable(minimum_vision_system tests/minimum_vision_system.cpp)
add_executable(communication_demo tests/communication_demo.cpp)

target_link_libraries(auto_aim_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim Threads::Threads)
target_link_libraries(camera_detect_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools auto_aim io)
target_link_libraries(camera_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
target_link_libraries(camera_thread_test PRIVATE ${OpenCV_LIBS} fmt::fmt auto_aim tools io)
target_link_libraries(cboard_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
target_link_libraries(fire_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
target_link_libraries(detector_video_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools auto_aim)
target_link_libraries(gimbal_response_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io)
target_link_libraries(multi_usbcamera_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
target_link_libraries(usbcamera_detect_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim)
target_link_libraries(usbcamera_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io)
target_link_libraries(handeye_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim)
target_link_libraries(dm_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io)
target_link_libraries(dm_virtual_sender_test PRIVATE fmt::fmt io)
target_link_libraries(minimum_vision_system PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim)
target_link_libraries(communication_demo PRIVATE io Threads::Threads)

add_executable(gimbal_test tests/gimbal_test.cpp)
target_link_libraries(gimbal_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)

add_executable(planner_test tests/planner_test.cpp)
target_link_libraries(planner_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)

add_executable(planner_test_offline tests/planner_test_offline.cpp)
target_link_libraries(planner_test_offline PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim tools io)

# 检查 ROS 环境
find_package(ament_cmake QUIET)
find_package(rclcpp QUIET)
find_package(std_msgs QUIET)
find_package(rosidl_typesupport_cpp QUIET)
find_package(sp_msgs QUIET)
# 只有在找到 ROS 环境时才会添加 sentry 和 publish_test
if(ament_cmake_FOUND AND rclcpp_FOUND AND std_msgs_FOUND AND rosidl_typesupport_cpp_FOUND AND sp_msgs_FOUND)
    # 编译 sentry 和 publish_test
    add_executable(sentry src/sentry.cpp)
    add_executable(sentry_bp src/sentry_bp.cpp)
    add_executable(sentry_debug src/sentry_debug.cpp)
    add_executable(sentry_multithread src/sentry_multithread.cpp)
    add_executable(publish_test tests/publish_test.cpp)
    add_executable(subscribe_test tests/subscribe_test.cpp)
    add_executable(topic_loop_test tests/topic_loop_test.cpp)
    add_executable(ws_bridge_test tests/ws_bridge_test.cpp)
    add_executable(dm_ws_bridge_test tests/dm_ws_bridge_test.cpp)

    target_link_libraries(sentry PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_bp PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_multithread PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(publish_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io auto_aim)
    target_link_libraries(subscribe_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
    target_link_libraries(topic_loop_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
    target_link_libraries(ws_bridge_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
    target_link_libraries(dm_ws_bridge_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
else()
    message(STATUS "ROS2 environment not found, skipping ROS2-related code.")
endif()

# 统一为所有核心库添加三方依赖（确保包含路径可见）
foreach(lib_target tools io auto_aim omniperception)
    if(TARGET ${lib_target})
        target_link_libraries(${lib_target} PUBLIC ${SP_THIRD_PARTY_TARGETS})
    endif()
endforeach()

set(SP_EXECUTABLE_TARGETS
    standard
    mt_standard
    standard_mpc
    auto_aim_debug_mpc
    mt_auto_aim_debug
    uav
    uav_debug
    capture
    calibrate_camera
    calibrate_handeye
    calibrate_robotworld_handeye
    split_video
    auto_aim_test
    camera_detect_test
    camera_test
    camera_thread_test
    cboard_test
    fire_test
    detector_video_test
    gimbal_response_test
    multi_usbcamera_test
    usbcamera_detect_test
    usbcamera_test
    handeye_test
    dm_test
    dm_virtual_sender_test
    minimum_vision_system
    gimbal_test
    planner_test
    planner_test_offline
    sentry
    sentry_bp
    sentry_debug
    sentry_multithread
    publish_test
    subscribe_test
    topic_loop_test
    ws_bridge_test
    dm_ws_bridge_test)

foreach(exe_target ${SP_EXECUTABLE_TARGETS})
    if(TARGET ${exe_target})
        target_link_libraries(${exe_target} PRIVATE ${SP_THIRD_PARTY_TARGETS})
    endif()
endforeach()
