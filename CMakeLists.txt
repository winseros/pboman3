cmake_minimum_required(VERSION 3.5)

project(pboman3 LANGUAGES CXX)

set(PBOM_BUILD_NUMBER "build \(ver\)" CACHE STRING "App build number")
set(PBOM_VERSION "version" CACHE STRING "App release version")

add_definitions(-DPBOM_PROJECT_NAME="PBO Manager V3")
add_definitions(-DPBOM_BUILD_NUMBER="${PBOM_BUILD_NUMBER}")
add_definitions(-DPBOM_VERSION="${PBOM_VERSION}")
add_definitions(-DPBOM_PROJECT_SITE="https://github.com/winseros/pboman3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(pbom)
add_subdirectory(pboe)

add_subdirectory(__lib__/cli11)
set(INSTALL_GTEST OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(__lib__/googletest)
