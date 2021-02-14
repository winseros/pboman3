cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Test REQUIRED)

set(PROJECT_SOURCES
        io/pbodatastream.cpp
        io/pbofile.cpp
        io/pboheaderio.cpp
        model/changestream.cpp
        model/pboentry.cpp
        model/pboheader.cpp
        model/pbomodel.cpp
        model/pbomodel2.cpp
        model/pbomodelevents.cpp
        model/pbonode.cpp
        model/pbonodeevents.cpp
        model/pbotreeexception.cpp
        mainwindow.cpp
        mainwindow.ui
        treemodel.cpp
        treenode.cpp
)

qt_add_resources(PROJECT_SOURCES res/res.qrc)
qt_add_executable(pbom ${PROJECT_SOURCES} main.cpp)
target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES
    "__test__/treenode_test.cpp"
    "io/__test__/pbofile_test.cpp"
    "io/__test__/pboheaderio_test.cpp"
    "model/__test__/pboentry_test.cpp"
    "model/__test__/pboheader_test.cpp"
    "model/__test__/pbomodel_test.cpp"
    "model/__test__/pbonode_test.cpp"
    "model/__test__/pbonode_events_tets.cpp"
)

add_executable(pbom_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
add_test(NAME pbom_test COMMAND pbom_test)
target_link_libraries(pbom_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gtest gmock)
target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
