cmake_minimum_required(VERSION 3.16.3)
# 设置CMake策略以消除OpenVINO警告
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()
project(sp_vision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
message(STATUS "--------------------CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}--------------------")

# Communication stack (uproto + MUX + DM-02 host driver)
add_subdirectory(third_party/Communication/modules EXCLUDE_FROM_ALL)

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

set(SP_THIRD_PARTY_TARGETS
    fmt::fmt
    spdlog::spdlog
    yaml-cpp
    nlohmann_json::nlohmann_json)

add_subdirectory(tools)
add_subdirectory(io)
add_subdirectory(tasks/auto_aim)
add_subdirectory(tasks/auto_buff)
add_subdirectory(tasks/omniperception)

####################src###################
add_executable(standard src/standard.cpp)
add_executable(mt_standard src/mt_standard.cpp)
add_executable(standard_mpc src/standard_mpc.cpp)
add_executable(auto_aim_debug_mpc src/auto_aim_debug_mpc.cpp)
add_executable(mt_auto_aim_debug src/mt_auto_aim_debug.cpp)
add_executable(auto_buff_debug src/auto_buff_debug.cpp)
add_executable(auto_buff_debug_mpc src/auto_buff_debug_mpc.cpp)
add_executable(uav src/uav.cpp)
add_executable(uav_debug src/uav_debug.cpp)

target_link_libraries(standard PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
target_link_libraries(mt_standard PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
target_link_libraries(standard_mpc PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
target_link_libraries(auto_aim_debug_mpc PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
target_link_libraries(mt_auto_aim_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
target_link_libraries(auto_buff_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_buff tools io ${CERES_LIBRARIES})
target_link_libraries(auto_buff_debug_mpc PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_buff tools io ${CERES_LIBRARIES})
target_link_libraries(uav PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim auto_buff tools io)
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
add_executable(auto_buff_test tests/auto_buff_test.cpp)
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
add_executable(minimum_vision_system tests/minimum_vision_system.cpp)
add_executable(communication_demo tests/communication_demo.cpp)

target_link_libraries(auto_aim_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim Holon::communication::drivers Threads::Threads)
target_link_libraries(auto_buff_test PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_buff tools io)
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
target_link_libraries(minimum_vision_system PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp tools io auto_aim)
target_link_libraries(communication_demo PRIVATE Holon::communication::drivers Threads::Threads)

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

    target_link_libraries(sentry PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_bp PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_debug PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(sentry_multithread PRIVATE ${OpenCV_LIBS} fmt::fmt yaml-cpp auto_aim omniperception tools io)
    target_link_libraries(publish_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io auto_aim)
    target_link_libraries(subscribe_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
    target_link_libraries(topic_loop_test PRIVATE ${OpenCV_LIBS} fmt::fmt tools io)
else()
    message(STATUS "ROS2 environment not found, skipping ROS2-related code.")
endif()

# 统一为所有核心库添加三方依赖（确保包含路径可见）
foreach(lib_target tools io auto_aim auto_buff omniperception)
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
    auto_buff_debug
    auto_buff_debug_mpc
    uav
    uav_debug
    capture
    calibrate_camera
    calibrate_handeye
    calibrate_robotworld_handeye
    split_video
    auto_aim_test
    auto_buff_test
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
    topic_loop_test)

foreach(exe_target ${SP_EXECUTABLE_TARGETS})
    if(TARGET ${exe_target})
        target_link_libraries(${exe_target} PRIVATE ${SP_THIRD_PARTY_TARGETS})
    endif()
endforeach()
