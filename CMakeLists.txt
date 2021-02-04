cmake_minimum_required(VERSION 3.5)

project(pboman3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(pbom)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(__lib__/googletest-release-1.10.0)
