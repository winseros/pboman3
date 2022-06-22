list(APPEND PROJECT_SOURCES_UI
    "ui/progresswidget/progresswidget.cpp"
    "ui/treewidget/deleteop.cpp"
    "ui/treewidget/treewidget.cpp"
    "ui/treewidget/treewidgetbase.cpp"
    "ui/treewidget/treewidgetitem.cpp"
    "ui/win32/win32iconmgr.cpp"
    "ui/win32/win32fileviewer.cpp"
    "ui/win32/win32taskbarindicator.cpp"
    "ui/aboutdialog.cpp"
    "ui/aboutdialog.ui"
    "ui/closedialog.cpp"
    "ui/closedialog.ui"
    "ui/compresslist.cpp"
    "ui/conflictslist.cpp"
    "ui/errordialog.ui"
    "ui/errordialog.cpp"
    "ui/fscollector.cpp"
    "ui/headersdialog.cpp"
    "ui/headersdialog.ui"
    "ui/insertdialog.cpp"
    "ui/insertdialog.ui"
    "ui/insertdialogbuttons.cpp"
    "ui/renamedialog.cpp"
    "ui/renamedialog.ui"
    "ui/mainwindow.cpp"
    "ui/mainwindow.ui"
    "ui/packwindow.cpp"
    "ui/signaturedialog.cpp"
    "ui/signaturedialog.ui"
    "ui/statusbar.cpp"
    "ui/statusbar.cpp"
    "ui/taskwindow.cpp"
    "ui/taskwindow.ui"
    "ui/unpackwindow.cpp"
    "ui/updatesdialog.ui"
    "ui/updatesdialog.cpp")

set(PROJECT_SOURCES_UI ${PROJECT_SOURCES_UI} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "ui/__test__/fscollector__test.cpp"
    "ui/__test__/updatesdialog__test.cpp"
    "ui/treewidget/__test__/treewidgetbase__test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
