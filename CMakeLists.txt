cmake_minimum_required(VERSION 3.5)

project(pboman3 LANGUAGES CXX)

add_definitions(-DPBOM_PROJECT_NAME="PBO Manager V3")
add_definitions(-DPBOM_BUILD_NUMBER="build \(ver\)")
add_definitions(-DPBOM_VERSION="version")
add_definitions(-DPBOM_PROJECT_SITE="https://github.com/winseros/pboman3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(pbom)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(__lib__/googletest)
