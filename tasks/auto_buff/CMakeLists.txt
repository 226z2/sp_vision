cmake_minimum_required(VERSION 3.16)

# 设置CMake策略以消除OpenVINO警告
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

set(OpenVINO_DIR "/opt/intel/openvino_2024.6.0/runtime/cmake")

find_package(OpenVINO REQUIRED)

# 强制使用 glog 而非 miniglog（避免符号冲突）
set(CERES_USE_MINIGLOG OFF CACHE BOOL "Use miniglog" FORCE)

find_package(Ceres REQUIRED)

add_library(auto_buff OBJECT 
    buff_type.cpp
    buff_detector.cpp
    buff_solver.cpp
    buff_aimer.cpp
    buff_target.cpp
    yolo11_buff.cpp
)

target_link_libraries(auto_buff PUBLIC auto_aim openvino::runtime ${CERES_LIBRARIES})
