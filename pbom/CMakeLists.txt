cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Test REQUIRED)

set(PROJECT_SOURCES
        io/bs/binarysource.cpp
        io/bs/fslzhbinarysource.cpp
        io/bs/fsrawbinarysource.cpp
        io/bs/pbobinarysource.cpp
        io/lzh/compressionbuffer.cpp
        io/lzh/compressionchunk.cpp
        io/lzh/decompressioncontext.cpp
        io/lzh/lzh.cpp
        io/lzh/lzhdecompressionexception.cpp
        io/binarybackend.cpp
        io/pbodatastream.cpp
        io/pbofile.cpp
        io/pboheaderio.cpp
        io/pboioexception.cpp
        model/interactionparcel.cpp
        model/pboentry.cpp
        model/pboheader.cpp
        model/pbomodel2.cpp
        model/pbomodelevents.cpp
        model/pbonode.cpp
        model/pbonodeevents.cpp
        model/pbopath.cpp
        model/pbotreeexception.cpp
        ui/busybar.cpp
        #ui/compressdialog.cpp
        #ui/compressdialog.ui
        #ui/compressionlistmodel.cpp
        ui/deleteop.cpp
        ui/fscollector.cpp
        ui/treewidget.cpp
        ui/treewidgetitem.cpp
        ui/mainwindow.cpp
        ui/mainwindow.ui
        util/appexception.cpp
)

qt_add_resources(PROJECT_SOURCES res/res.qrc)
qt_add_executable(pbom ${PROJECT_SOURCES} main.cpp)
target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(TEST_SOURCES
    "io/bs/__test__/fslzhbinarysource_test.cpp"
    "io/bs/__test__/fsrawbinarysource_test.cpp"
    "io/bs/__test__/pbobinarysource_test.cpp"
    "io/lzh/__test__/compressionbuffer_test.cpp"
    "io/lzh/__test__/compressionchunk_test.cpp"
    "io/lzh/__test__/lzh_test.cpp"
    "io/__test__/binarybackend_test.cpp"
    "io/__test__/pbofile_test.cpp"
    "io/__test__/pboheaderio_test.cpp"
    "model/__test__/interactionparcel_test.cpp"
    "model/__test__/pboentry_test.cpp"
    "model/__test__/pboheader_test.cpp"
    "model/__test__/pbonode_test.cpp"
    "model/__test__/pbonode_events_tets.cpp"
    "model/__test__/pbopath_test.cpp"
    "ui/__test__/fscollector__test.cpp"
 )

add_executable(pbom_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
add_test(NAME pbom_test COMMAND pbom_test)
target_link_libraries(pbom_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gtest gmock)
target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(pbom_test PRIVATE SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
