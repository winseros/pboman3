set(PROJECT_SOURCES
    commands/explorercommand.cpp
    commands/fileselectionenumerator.cpp
    commands/folderselectionenumerator.cpp
    commands/packascommand.cpp
    commands/packincommand.cpp
    commands/packtocommand.cpp
    commands/unpackascommand.cpp
    commands/unpackincommand.cpp
    commands/unpacktocommand.cpp
    dllmain.cpp
    executable.cpp
    helpers.cpp
    registry.cpp
)

add_library(pboe MODULE ${PROJECT_SOURCES})
target_link_libraries(pboe PRIVATE shlwapi RuntimeObject)
target_compile_definitions(pboe PRIVATE
    PBOM_EXECUTABLE="pbom.exe"
    UNICODE)

add_library(pboe_s STATIC ${PROJECT_SOURCES})
target_link_libraries(pboe_s PRIVATE shlwapi RuntimeObject)
target_compile_definitions(pboe_s PRIVATE
    PBOM_EXECUTABLE="pbom.exe"
    UNICODE)

if(MSVC)
  set(CMAKE_STATIC_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set(CMAKE_MODULE_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /DEF:${CMAKE_CURRENT_SOURCE_DIR}/dllmain.def /NODEFAULTLIB:LIBCMT")
endif()

include(GNUInstallDirs)
install(TARGETS pboe DESTINATION ${CMAKE_INSTALL_BINDIR})

set(TEST_SOURCES
    __test__/dllmain_test.cpp
 )

add_executable(pboe_test ${TEST_SOURCES} testmain.cpp)
target_link_libraries(pboe_test PRIVATE gtest gmock shlwapi pboe_s)
target_include_directories(pboe_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pboe_test PRIVATE BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
