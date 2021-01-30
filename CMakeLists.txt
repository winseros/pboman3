cmake_minimum_required(VERSION 3.5)

project(pboman3 LANGUAGES CXX)

cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        src/io/pbodatastream.cpp
        src/io/pbofile.cpp
        src/io/pboheaderreader.cpp
        src/model/pboentry.cpp
        src/model/pboheader.cpp
        src/model/pbomodel.cpp
        src/model/pbomodelevents.cpp
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.ui
        src/treemodel.cpp
)

qt_add_executable(pbom ${PROJECT_SOURCES})
target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}\\src)
