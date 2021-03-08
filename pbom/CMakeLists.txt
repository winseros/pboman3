cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Test REQUIRED)

set(PROJECT_SOURCES
        io/binarybackend.cpp
        io/binarysource.cpp
        io/pbodatastream.cpp
        io/pbofile.cpp
        io/pboheaderio.cpp
        io/pboioexception.cpp
        model/parcelmanager.cpp
        model/pboentry.cpp
        model/pboheader.cpp
        model/pbomodel2.cpp
        model/pbomodelevents.cpp
        model/pbonode.cpp
        model/pbonodeevents.cpp
        model/pboparcel.cpp
        model/pbopath.cpp
        model/pbotreeexception.cpp
        ui/treewidget.cpp
        ui/treewidgetitem.cpp
        ui/mainwindow.cpp
        ui/mainwindow.ui
)

qt_add_resources(PROJECT_SOURCES res/res.qrc)
qt_add_executable(pbom ${PROJECT_SOURCES} main.cpp)
target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES
    "io/__test__/binarysource_test.cpp"
    "io/__test__/binarybackend_test.cpp"
    "io/__test__/pbofile_test.cpp"
    "io/__test__/pboheaderio_test.cpp"
    "model/__test__/parcelmanager_test.cpp"
    "model/__test__/pboentry_test.cpp"
    "model/__test__/pboheader_test.cpp"
    "model/__test__/pbonode_test.cpp"
    "model/__test__/pbonode_events_tets.cpp"
    "model/__test__/pboparcel_test.cpp"
    "model/__test__/pbopath_test.cpp"
 )

add_executable(pbom_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
add_test(NAME pbom_test COMMAND pbom_test)
target_link_libraries(pbom_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gtest gmock)
target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
