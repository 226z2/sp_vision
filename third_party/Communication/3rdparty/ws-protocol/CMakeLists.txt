cmake_minimum_required(VERSION 3.15)
project(FoxgloveWebSocket CXX)

find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
  find_path(nlohmann_json_INCLUDE_DIRS "nlohmann/json.hpp"
    HINTS
      ${CMAKE_SOURCE_DIR}/.cache/json/include
      ${CMAKE_SOURCE_DIR}/.cache/json/single_include
      ${CMAKE_SOURCE_DIR}/../.cache/json/include
      ${CMAKE_SOURCE_DIR}/../.cache/json/single_include
    PATHS
      /usr/include /usr/local/include
  )
  if(NOT nlohmann_json_INCLUDE_DIRS)
    message(FATAL_ERROR "Could not find nlohmann_json (nlohmann/json.hpp). Please install it or set CMAKE_PREFIX_PATH.")
  endif()
  add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
  set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${nlohmann_json_INCLUDE_DIRS}"
  )
endif()

if(NOT WEBSOCKETPP_INCLUDE_DIR)
  find_path(WEBSOCKETPP_INCLUDE_DIR "websocketpp/config/asio_no_tls.hpp"
    HINTS
      ${CMAKE_SOURCE_DIR}/.cache/websocketpp
      ${CMAKE_SOURCE_DIR}/../.cache/websocketpp
      ${CMAKE_SOURCE_DIR}/.cache/websocketpp/websocketpp
      ${CMAKE_SOURCE_DIR}/../.cache/websocketpp/websocketpp
    PATHS
      /usr/include /usr/local/include
  )
endif()
if(NOT WEBSOCKETPP_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find websocketpp headers (websocketpp/config/asio_no_tls.hpp). Run `dfx deps sync` or install websocketpp.")
endif()
message(STATUS "Using websocketpp at: ${WEBSOCKETPP_INCLUDE_DIR}")

add_library(foxglove_websocket src/base64.cpp src/parameter.cpp src/serialization.cpp src/server_factory.cpp)
target_include_directories(foxglove_websocket
    PUBLIC 
        include
        ${WEBSOCKETPP_INCLUDE_DIR}
    PRIVATE
        ${nlohmann_json_INCLUDE_DIRS}
)

# 设置websocketpp为SYSTEM以屏蔽警告
target_include_directories(foxglove_websocket SYSTEM PUBLIC ${WEBSOCKETPP_INCLUDE_DIR})
target_link_libraries(foxglove_websocket PUBLIC nlohmann_json::nlohmann_json)
set_target_properties(foxglove_websocket PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

if (MSVC)
  target_compile_options(foxglove_websocket PRIVATE /W4)
else()
  target_compile_options(foxglove_websocket PRIVATE -Wall -Wextra -Wpedantic -Wold-style-cast -Wfloat-equal)
endif()

install(TARGETS foxglove_websocket)
INSTALL (DIRECTORY ${CMAKE_SOURCE_DIR}/include/
         DESTINATION include)
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX}/licenses/)
