cmake_minimum_required(VERSION 3.15)
project(FoxgloveWebSocket CXX)

# Prefer package configs; fall back to header-only includes if not found
find_package(nlohmann_json QUIET)
find_package(websocketpp QUIET)

if(NOT nlohmann_json_FOUND)
  message(WARNING "nlohmann_json not found via find_package; attempting header-only include lookup")
  find_path(NLOHMANN_JSON_INCLUDE_DIR NAMES nlohmann/json.hpp PATHS
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../../json/include
    ${CMAKE_SOURCE_DIR}/../../../json/include
    /usr/include /usr/local/include
  )
  if(NOT NLOHMANN_JSON_INCLUDE_DIR)
    message(FATAL_ERROR "nlohmann_json headers not found (nlohmann/json.hpp)")
  endif()
endif()
if(NOT websocketpp_FOUND)
  message(WARNING "websocketpp not found via find_package; attempting header-only include lookup")
  find_path(WEBSOCKETPP_INCLUDE_DIR NAMES websocketpp/config/asio_no_tls.hpp PATHS
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../../websocketpp
    /usr/include /usr/local/include
  )
  if(NOT WEBSOCKETPP_INCLUDE_DIR)
    message(FATAL_ERROR "websocketpp headers not found")
  endif()
endif()

add_library(foxglove_websocket src/base64.cpp src/parameter.cpp src/serialization.cpp src/server_factory.cpp)
target_include_directories(foxglove_websocket
    PUBLIC include
)
if(nlohmann_json_FOUND)
  target_link_libraries(foxglove_websocket nlohmann_json::nlohmann_json)
else()
  target_include_directories(foxglove_websocket SYSTEM PUBLIC ${NLOHMANN_JSON_INCLUDE_DIR})
endif()
if(websocketpp_FOUND)
  target_link_libraries(foxglove_websocket websocketpp::websocketpp)
else()
  target_include_directories(foxglove_websocket SYSTEM PUBLIC ${WEBSOCKETPP_INCLUDE_DIR})
endif()
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
