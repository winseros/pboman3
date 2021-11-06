cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(WIN32_VER 0x0A00) #https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets Network Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network Test REQUIRED)

add_subdirectory(domain)
add_subdirectory(io)
add_subdirectory(model)
add_subdirectory(ui)
add_subdirectory(util)

qt_add_resources(PROJECT_SOURCES res/res.qrc)
qt_add_executable(pbom ${PROJECT_SOURCES} main.cpp res/winapp.rc)

target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network CLI11)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pbom PRIVATE WINVER=${WIN32_VER} NOMINMAX)

SET(QT_BINARIES_DIR ${Qt${QT_VERSION_MAJOR}_DIR}/../../..)
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(QT_BINARIES_SUFFIX)
    set_target_properties(pbom PROPERTIES WIN32_EXECUTABLE TRUE)
else()
    set(QT_BINARIES_SUFFIX d)
endif()

include(GNUInstallDirs)
install(TARGETS pbom)
install(FILES 
    ${QT_BINARIES_DIR}/bin/Qt6Core${QT_BINARIES_SUFFIX}.dll
    ${QT_BINARIES_DIR}/bin/Qt6Gui${QT_BINARIES_SUFFIX}.dll
    ${QT_BINARIES_DIR}/bin/Qt6Widgets${QT_BINARIES_SUFFIX}.dll
    ${QT_BINARIES_DIR}/bin/Qt6Network${QT_BINARIES_SUFFIX}.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES 
    ${QT_BINARIES_DIR}/plugins/imageformats/qico${QT_BINARIES_SUFFIX}.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR}/imageformats)
install(FILES 
    ${QT_BINARIES_DIR}/plugins/platforms/qwindows${QT_BINARIES_SUFFIX}.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR}/platforms)
install(FILES 
    ${QT_BINARIES_DIR}/plugins/styles/qwindowsvistastyle${QT_BINARIES_SUFFIX}.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR}/styles)
install(FILES 
    ${QT_BINARIES_DIR}/../../Tools/OpenSSL/Win_x64/bin/libssl-1_1-x64.dll
    ${QT_BINARIES_DIR}/../../Tools/OpenSSL/Win_x64/bin/libcrypto-1_1-x64.dll
    DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(pbom_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
add_test(NAME pbom_test COMMAND pbom_test)
target_link_libraries(pbom_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network gtest gmock)
target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(pbom_test PRIVATE 
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    WINVER=${WIN32_VER})
