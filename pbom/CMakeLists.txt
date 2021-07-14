cmake_policy(SET CMP0074 NEW)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (MSVC)
    add_compile_options(/Zc:preprocessor)
endif()

set(WIN32_VER 0x0A00) #https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160

set(CMAKE_FIND_DEBUG_MODE FALSE)
find_package(QT NAMES Qt6 COMPONENTS Widgets Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Test REQUIRED)

set(PROJECT_SOURCES
        io/bb/binarybackend.cpp
        io/bb/execbackend.cpp
        io/bb/nodefilesystem.cpp
        io/bb/tempbackend.cpp
        io/bb/unpackbackend.cpp
        io/bs/binarysource.cpp
        io/bs/fslzhbinarysource.cpp
        io/bs/fsrawbinarysource.cpp
        io/bs/pbobinarysource.cpp
        io/lzh/compressionbuffer.cpp
        io/lzh/compressionchunk.cpp
        io/lzh/decompressioncontext.cpp
        io/lzh/lzh.cpp
        io/lzh/lzhdecompressionexception.cpp
        io/pbodatastream.cpp
        io/pbofile.cpp
        io/pboheaderio.cpp
        io/pboheaderreader.cpp
        io/pboioexception.cpp
        io/pbowriter.cpp
        model/conflictsparcel.cpp
        model/diskaccessexception.cpp
        model/headersmodel.cpp
        model/interactionparcel.cpp
        model/pboentry.cpp
        model/pbofileformatexception.cpp
        model/pboheader.cpp
        model/pbomodel.cpp
        model/pbonode.cpp
        model/pbonodeevents.cpp
        model/pbopath.cpp
        model/signaturemodel.cpp
        ui/treewidget/deleteop.cpp
        ui/treewidget/treewidget.cpp
        ui/treewidget/treewidgetbase.cpp
        ui/treewidget/treewidgetitem.cpp
        ui/win32/win32iconmgr.cpp
        ui/win32/win32fileviewer.cpp
        ui/aboutdialog.cpp
        ui/aboutdialog.ui
        ui/closedialog.cpp
        ui/closedialog.ui
        ui/compresslist.cpp
        ui/conflictslist.cpp
        ui/errordialog.ui
        ui/errordialog.cpp
        ui/fscollector.cpp
        ui/headersdialog.cpp
        ui/headersdialog.ui
        ui/insertdialog.cpp
        ui/insertdialog.ui
        ui/insertdialogbuttons.cpp
        ui/renamedialog.cpp
        ui/renamedialog.ui
        ui/mainwindow.cpp
        ui/mainwindow.ui
        ui/signaturedialog.cpp
        ui/signaturedialog.ui
        ui/statusbar.cpp
        util/exception.cpp
        util/log.cpp
        util/util.cpp
)

qt_add_resources(PROJECT_SOURCES res/res.qrc)
qt_add_executable(pbom ${PROJECT_SOURCES} main.cpp res/winapp.rc)
target_link_libraries(pbom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(pbom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(pbom PRIVATE WINVER=${WIN32_VER})

SET(QT_BINARIES_DIR ${Qt${QT_VERSION_MAJOR}_DIR}/../../..)
if(CMAKE_BUILD_TYPE MATCHES Release)
    set(QT_BINARIES_SUFFIX)
else()
    set(QT_BINARIES_SUFFIX d)
endif()

include(GNUInstallDirs)
install(TARGETS pbom)
install(FILES 
    ${QT_BINARIES_DIR}/bin/Qt6Core${QT_BINARIES_SUFFIX}.dll
    ${QT_BINARIES_DIR}/bin/Qt6Gui${QT_BINARIES_SUFFIX}.dll
    ${QT_BINARIES_DIR}/bin/Qt6Widgets${QT_BINARIES_SUFFIX}.dll
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

set(TEST_SOURCES
    "io/bb/__test__/execbackend_test.cpp"
    "io/bb/__test__/tempbackend_test.cpp"
    "io/bb/__test__/unpackbackend_test.cpp"
    "io/bs/__test__/fslzhbinarysource_test.cpp"
    "io/bs/__test__/fsrawbinarysource_test.cpp"
    "io/bs/__test__/pbobinarysource_test.cpp"
    "io/lzh/__test__/compressionbuffer_test.cpp"
    "io/lzh/__test__/compressionchunk_test.cpp"
    "io/lzh/__test__/lzh_test.cpp"
    "io/__test__/pbofile_test.cpp"
    "io/__test__/pboheaderio_test.cpp"
    "io/__test__/pboheaderreader_test.cpp"
    "io/__test__/pbowriter_test.cpp"
    "model/__test__/conflictsparcel_test.cpp"
    "model/__test__/headersmodel_test.cpp"
    "model/__test__/interactionparcel_test.cpp"
    "model/__test__/pboentry_test.cpp"
    "model/__test__/pboheader_test.cpp"
    "model/__test__/pbonode_test.cpp"
    "model/__test__/pbopath_test.cpp"
    "model/__test__/signaturemodel_test.cpp"
    "ui/__test__/fscollector__test.cpp"
    "ui/treewidget/__test__/treewidgetbase__test.cpp"
    "util/__test__/qpointerlistiterator_test.cpp"
    "util/__test__/util_test.cpp"
 )

add_executable(pbom_test ${PROJECT_SOURCES} ${TEST_SOURCES} testmain.cpp)
add_test(NAME pbom_test COMMAND pbom_test)
target_link_libraries(pbom_test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets gtest gmock)
target_include_directories(pbom_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(pbom_test PRIVATE 
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    WINVER=${WIN32_VER})
