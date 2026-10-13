cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(WIN32_VER 0x0A00) #https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160

set(CMAKE_FIND_DEBUG_MODE FALSE)
set(PBOM_QT_COMPONENTS Core)
if(PBOM_BUILD_GUI OR BUILD_TESTING)
    list(APPEND PBOM_QT_COMPONENTS Concurrent Network Widgets)
endif()
if(BUILD_TESTING)
    list(APPEND PBOM_QT_COMPONENTS Test)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${PBOM_QT_COMPONENTS})

add_subdirectory(domain)
add_subdirectory(io)
add_subdirectory(model)
add_subdirectory(settings)
add_subdirectory(util)

if(PBOM_BUILD_GUI OR BUILD_TESTING)
    add_subdirectory(ui)
endif()

add_library(pbom_core STATIC ${PROJECT_SOURCES} exception.cpp)
target_include_directories(pbom_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pbom_core PUBLIC Qt6::Core)
target_compile_definitions(pbom_core PRIVATE NOMINMAX)

if(PBOM_BUILD_GUI OR BUILD_TESTING)
    add_library(pbom_ui STATIC ${PROJECT_SOURCES_UI})
    target_include_directories(pbom_ui
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/pbom_ui_autogen/include>
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ui)
    target_link_libraries(pbom_ui
        PUBLIC pbom_core Qt6::Concurrent Qt6::Network Qt6::Widgets)
    target_compile_definitions(pbom_ui PRIVATE WINVER=${WIN32_VER} NOMINMAX)
endif()

if(PBOM_BUILD_GUI)
    set(PBOM_GUI_SOURCES main.cpp)
    if(WIN32)
        list(APPEND PBOM_GUI_SOURCES win32com.cpp res/winapp.rc)
    endif()
    qt_add_resources(PBOM_GUI_SOURCES res/res.qrc)

    qt_add_executable(pbom ${PBOM_GUI_SOURCES})
    target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(pbom PRIVATE pbom_ui CLI11)
    target_compile_definitions(pbom PRIVATE WINVER=${WIN32_VER} NOMINMAX PBOM_GUI)
    set_target_properties(pbom PROPERTIES WIN32_EXECUTABLE "$<CONFIG:Release>")

    install(TARGETS pbom RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(PBOM_BUILD_CLI)
    set(PBOM_CLI_SOURCES console.cpp)
    if(WIN32)
        list(APPEND PBOM_CLI_SOURCES res/console.rc)
    endif()

    add_executable(pboc ${PBOM_CLI_SOURCES})
    target_include_directories(pboc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(pboc PRIVATE pbom_core CLI11)
    target_compile_definitions(pboc PRIVATE NOMINMAX)

    install(TARGETS pboc RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if(WIN32)
    set(QT_BINARIES_DIR "${Qt6_DIR}/../../..")
    if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
        set(QT_BINARIES_SUFFIX "")
    else()
        set(QT_BINARIES_SUFFIX "d")
    endif()

    install(FILES ${QT_BINARIES_DIR}/bin/Qt6Core${QT_BINARIES_SUFFIX}.dll
        DESTINATION ${CMAKE_INSTALL_BINDIR})

    if(PBOM_BUILD_GUI)
        install(FILES
            ${QT_BINARIES_DIR}/bin/Qt6Gui${QT_BINARIES_SUFFIX}.dll
            ${QT_BINARIES_DIR}/bin/Qt6Widgets${QT_BINARIES_SUFFIX}.dll
            ${QT_BINARIES_DIR}/bin/Qt6Network${QT_BINARIES_SUFFIX}.dll
            ${QT_BINARIES_DIR}/bin/Qt6Concurrent${QT_BINARIES_SUFFIX}.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(FILES ${QT_BINARIES_DIR}/plugins/imageformats/qico${QT_BINARIES_SUFFIX}.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}/imageformats)
        install(FILES ${QT_BINARIES_DIR}/plugins/platforms/qwindows${QT_BINARIES_SUFFIX}.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}/platforms)
        install(FILES ${QT_BINARIES_DIR}/plugins/styles/qmodernwindowsstyle${QT_BINARIES_SUFFIX}.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}/styles)
        install(FILES
            ${QT_BINARIES_DIR}/plugins/tls/qcertonlybackend${QT_BINARIES_SUFFIX}.dll
            ${QT_BINARIES_DIR}/plugins/tls/qopensslbackend${QT_BINARIES_SUFFIX}.dll
            ${QT_BINARIES_DIR}/plugins/tls/qschannelbackend${QT_BINARIES_SUFFIX}.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}/tls)
    endif()
endif()

if(BUILD_TESTING)
    set(PBOM_TEST_SOURCES
        ${TEST_SOURCES}
        __test__/commandline_test.cpp
        testmain.cpp)
    qt_add_resources(PBOM_TEST_SOURCES res/res.qrc)

    add_executable(pbom_test ${PBOM_TEST_SOURCES})
    target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(pbom_test
        PRIVATE pbom_ui Qt6::Test CLI11 gtest gmock)
    target_compile_definitions(pbom_test PRIVATE
        SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        WINVER=${WIN32_VER}
        NOMINMAX)

    add_test(NAME pbom_test COMMAND pbom_test)
    set_tests_properties(pbom_test PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
