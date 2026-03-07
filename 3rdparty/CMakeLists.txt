cmake_minimum_required(VERSION 3.20)

include(FetchContent)

# NOTE:
# - This project is expected to be built inside the provided dev container where deps
#   are preinstalled under /opt/* (no network required).
# - FetchContent is kept as an explicit opt-in fallback.
option(PRINTK_VISON_3RDPARTY_FETCH "Fetch 3rdparty deps via FetchContent (may require network)" OFF)
option(PRINTK_VISON_3RDPARTY_PREFER_SYSTEM "Prefer system-installed packages when available" ON)

# Local third-party libs bundled in the repo.

# Dev-container convenience: if deps are installed under /opt/* but the user
# didn't export CMAKE_PREFIX_PATH, help CMake discover them.
set(_printk_vison_added_prefixes "")
foreach(_opt_prefix IN ITEMS /opt/spdlog /opt/json /opt/yaml-cpp /opt/flatbuffers /opt/websocketpp /opt/asio /opt/opencv)
  if(EXISTS "${_opt_prefix}/lib/cmake" OR EXISTS "${_opt_prefix}/lib64/cmake" OR EXISTS "${_opt_prefix}/share/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${_opt_prefix}")
    list(APPEND _printk_vison_added_prefixes "${_opt_prefix}")
  endif()
endforeach()
if(_printk_vison_added_prefixes)
  message(STATUS "printk_vison/3rdparty: CMAKE_PREFIX_PATH += ${_printk_vison_added_prefixes}")
endif()

function(_printk_vison_import_yaml_cpp_from_opt)
  if(TARGET yaml-cpp::yaml-cpp OR TARGET yaml-cpp)
    return()
  endif()
  if(NOT EXISTS "/opt/yaml-cpp/include/yaml-cpp/yaml.h")
    return()
  endif()
  set(_YAML_CPP_LIB "")
  find_library(_YAML_CPP_LIB NAMES yaml-cpp
    HINTS /opt/yaml-cpp/lib /opt/yaml-cpp/lib64 /opt/yaml-cpp/lib/x86_64-linux-gnu
    PATH_SUFFIXES "" x86_64-linux-gnu
  )
  if(NOT _YAML_CPP_LIB)
    # Some installs only ship versioned sonames without an unversioned symlink.
    file(GLOB _yamlcpp_candidates
      "/opt/yaml-cpp/lib/libyaml-cpp.so*"
      "/opt/yaml-cpp/lib64/libyaml-cpp.so*"
      "/opt/yaml-cpp/lib/x86_64-linux-gnu/libyaml-cpp.so*"
      "/opt/yaml-cpp/lib/libyaml-cpp.a"
      "/opt/yaml-cpp/lib64/libyaml-cpp.a"
      "/opt/yaml-cpp/lib/x86_64-linux-gnu/libyaml-cpp.a"
    )
    list(FILTER _yamlcpp_candidates INCLUDE REGEX "libyaml-cpp\\.(so|a)")
    list(LENGTH _yamlcpp_candidates _yamlcpp_n)
    if(_yamlcpp_n GREATER 0)
      list(GET _yamlcpp_candidates 0 _YAML_CPP_LIB)
    endif()
  endif()
  if(NOT _YAML_CPP_LIB)
    message(WARNING "printk_vison/3rdparty: /opt/yaml-cpp headers found but libyaml-cpp not found under /opt/yaml-cpp/lib*")
    return()
  endif()
  add_library(yaml-cpp::yaml-cpp UNKNOWN IMPORTED)
  set_target_properties(yaml-cpp::yaml-cpp PROPERTIES
    IMPORTED_LOCATION "${_YAML_CPP_LIB}"
    INTERFACE_INCLUDE_DIRECTORIES "/opt/yaml-cpp/include"
  )
  message(STATUS "printk_vison/3rdparty: yaml-cpp -> /opt/yaml-cpp (imported: ${_YAML_CPP_LIB})")
endfunction()

function(_printk_vison_import_nlohmann_json_from_opt)
  if(TARGET nlohmann_json::nlohmann_json)
    return()
  endif()
  if(NOT EXISTS "/opt/json/include/nlohmann/json.hpp")
    return()
  endif()
  add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
  set_target_properties(nlohmann_json::nlohmann_json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "/opt/json/include"
  )
  message(STATUS "printk_vison/3rdparty: nlohmann_json -> /opt/json (imported headers)")
endfunction()

function(_printk_vison_import_spdlog_from_opt)
  if(TARGET spdlog::spdlog OR TARGET spdlog::spdlog_header_only)
    return()
  endif()
  if(NOT EXISTS "/opt/spdlog/include/spdlog/spdlog.h")
    return()
  endif()
  add_library(spdlog::spdlog_header_only INTERFACE IMPORTED)
  set_target_properties(spdlog::spdlog_header_only PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "/opt/spdlog/include"
  )
  message(STATUS "printk_vison/3rdparty: spdlog -> /opt/spdlog (imported headers)")
endfunction()

function(_printk_vison_import_flatbuffers_from_opt)
  if(TARGET flatbuffers::flatbuffers)
    return()
  endif()
  if(NOT EXISTS "/opt/flatbuffers/include/flatbuffers/flatbuffers.h")
    return()
  endif()
  add_library(flatbuffers::flatbuffers INTERFACE IMPORTED)
  set_target_properties(flatbuffers::flatbuffers PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "/opt/flatbuffers/include"
  )
  message(STATUS "printk_vison/3rdparty: flatbuffers -> /opt/flatbuffers (imported headers)")
endfunction()

set(_printk_vison_found_yamlcpp OFF)
set(_printk_vison_found_json OFF)
set(_printk_vison_found_spdlog OFF)
set(_printk_vison_found_flatbuffers OFF)

add_subdirectory(tinympc)

if(PRINTK_VISON_3RDPARTY_PREFER_SYSTEM)
  find_package(spdlog QUIET)
  find_package(nlohmann_json QUIET)
  find_package(yaml-cpp QUIET)
  find_package(flatbuffers QUIET)
endif()

_printk_vison_import_spdlog_from_opt()
_printk_vison_import_nlohmann_json_from_opt()
_printk_vison_import_yaml_cpp_from_opt()
_printk_vison_import_flatbuffers_from_opt()

if(TARGET yaml-cpp AND NOT TARGET yaml-cpp::yaml-cpp)
  add_library(yaml-cpp::yaml-cpp ALIAS yaml-cpp)
endif()

if(TARGET spdlog::spdlog OR TARGET spdlog::spdlog_header_only)
  set(_printk_vison_found_spdlog ON)
  message(STATUS "printk_vison/3rdparty: spdlog -> found (target exists)")
endif()
if(TARGET nlohmann_json::nlohmann_json)
  set(_printk_vison_found_json ON)
  message(STATUS "printk_vison/3rdparty: nlohmann_json -> found (target exists)")
endif()
if(TARGET yaml-cpp::yaml-cpp OR TARGET yaml-cpp)
  set(_printk_vison_found_yamlcpp ON)
  message(STATUS "printk_vison/3rdparty: yaml-cpp -> found (target exists)")
endif()
if(TARGET flatbuffers::flatbuffers)
  set(_printk_vison_found_flatbuffers ON)
  message(STATUS "printk_vison/3rdparty: flatbuffers -> found (target exists)")
endif()

if(PRINTK_VISON_3RDPARTY_FETCH)
  if(NOT _printk_vison_found_spdlog)
    message(STATUS "printk_vison/3rdparty: spdlog -> FetchContent (may require network)")
    FetchContent_Declare(
      spdlog
      GIT_REPOSITORY https://github.com/gabime/spdlog.git
      GIT_TAG v1.15.3
      GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(spdlog)
  endif()

  if(NOT _printk_vison_found_json)
    message(STATUS "printk_vison/3rdparty: nlohmann_json -> FetchContent (may require network)")
    FetchContent_Declare(
      json
      GIT_REPOSITORY https://github.com/nlohmann/json
      GIT_TAG v3.12.0 # 55f93686c01528224f448c19128836e7df245f72
      GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(json)
  endif()

  if(NOT _printk_vison_found_yamlcpp)
    message(STATUS "printk_vison/3rdparty: yaml-cpp -> FetchContent (may require network)")
    FetchContent_Declare(
      yaml-cpp
      GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
      GIT_TAG yaml-cpp-0.7.0
      GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(yaml-cpp)
  endif()
else()
  # No FetchContent: require deps from the system (/opt in the dev container).
  if(NOT _printk_vison_found_yamlcpp)
    message(FATAL_ERROR "printk_vison/3rdparty: yaml-cpp not found. Expected /opt/yaml-cpp (dev container) or enable -DPRINTK_VISON_3RDPARTY_FETCH=ON.")
  endif()
  if(NOT _printk_vison_found_json)
    message(FATAL_ERROR "printk_vison/3rdparty: nlohmann_json not found. Expected /opt/json (dev container) or enable -DPRINTK_VISON_3RDPARTY_FETCH=ON.")
  endif()
  if(NOT _printk_vison_found_flatbuffers)
    message(FATAL_ERROR "printk_vison/3rdparty: flatbuffers not found. Expected /opt/flatbuffers (dev container) or enable -DPRINTK_VISON_3RDPARTY_FETCH=ON.")
  endif()
endif()
