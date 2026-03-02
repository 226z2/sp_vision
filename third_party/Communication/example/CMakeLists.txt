cmake_minimum_required(VERSION 3.20)

# Examples are optional and may be empty in some workspaces.
# Keep this file so top-level `add_subdirectory(example)` never fails.

foreach(d demo01 demo02 camera flow_minimal communication)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${d}/CMakeLists.txt")
        add_subdirectory(${d})
    endif()
endforeach()
