cmake_minimum_required(VERSION 3.14)

project(pboman3 LANGUAGES CXX)

include(CTest)
include(GNUInstallDirs)

set(PBOM_BUILD_NUMBER "build \(ver\)" CACHE STRING "App build number")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" PBOM_DEFAULT_VERSION LIMIT_COUNT 1)
set(PBOM_VERSION "${PBOM_DEFAULT_VERSION}" CACHE STRING "App release version")

option(PBOM_BUILD_GUI "Build the PBO Manager desktop application" ON)
option(PBOM_BUILD_CLI "Build the PBO Manager command-line application" ON)

set(_PBOM_EXPLORER_DEFAULT OFF)
if(WIN32)
    set(_PBOM_EXPLORER_DEFAULT ON)
endif()
option(PBOM_BUILD_EXPLORER_EXTENSION "Build the Windows Explorer extension" ${_PBOM_EXPLORER_DEFAULT})

if(NOT PBOM_BUILD_GUI AND NOT PBOM_BUILD_CLI)
    message(FATAL_ERROR "At least one of PBOM_BUILD_GUI or PBOM_BUILD_CLI must be enabled")
endif()

add_compile_definitions(
    PBOM_PROJECT_NAME="PBO Manager"
    PBOM_BUILD_NUMBER="${PBOM_BUILD_NUMBER}"
    PBOM_VERSION="${PBOM_VERSION}"
    PBOM_PROJECT_SITE="https://github.com/winseros/pboman3"
    PBOM_API_SITE="https://api.github.com/repos/winseros/pboman3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

add_subdirectory(__lib__/cli11 EXCLUDE_FROM_ALL)

if(BUILD_TESTING)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(__lib__/googletest EXCLUDE_FROM_ALL)
endif()

add_subdirectory(pbom)

if(UNIX AND NOT APPLE AND PBOM_BUILD_GUI)
    add_subdirectory(linux)
endif()

if(WIN32 AND PBOM_BUILD_EXPLORER_EXTENSION)
    add_subdirectory(pboe)
endif()
