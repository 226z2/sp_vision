cmake_minimum_required(VERSION 3.16)

# 设置CMake策略以消除OpenVINO警告
if(POLICY CMP0144)
    cmake_policy(SET CMP0144 NEW)
endif()

set(OpenVINO_DIR "/opt/intel/openvino_2024.6.0/runtime/cmake")

find_package(OpenVINO REQUIRED COMPONENTS Runtime)

add_library(omniperception OBJECT 
    decider.cpp
    perceptron.cpp
)
target_link_libraries(omniperception PUBLIC openvino::runtime)
