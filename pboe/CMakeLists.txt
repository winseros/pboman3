set(PROJECT_SOURCES
    dllmain.cpp
    classfactory.cpp
    contextmenu.cpp
    registry.cpp
)

add_library(pboe MODULE ${PROJECT_SOURCES})
target_link_libraries(pboe PRIVATE shlwapi)
add_library(pboe_s STATIC ${PROJECT_SOURCES})
target_link_libraries(pboe_s PRIVATE shlwapi)

if(MSVC)
  set(CMAKE_STATIC_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set(CMAKE_MODULE_LINKER_FLAGS
      "${CMAKE_MODULE_LINKER_FLAGS} /DEF:${CMAKE_CURRENT_SOURCE_DIR}/dllmain.def /NODEFAULTLIB:LIBCMT")
endif()


set(TEST_SOURCES
    __test__/dllmain_test.cpp
 )

add_executable(pboe_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
target_link_libraries(pboe_test PRIVATE gtest gmock shlwapi)
target_include_directories(pboe_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
