cmake_minimum_required(VERSION 3.20)

add_executable(communication_demo
    main.cpp
)

target_compile_features(communication_demo PUBLIC cxx_std_20)
target_link_libraries(communication_demo PRIVATE Holon::communication::drivers)
