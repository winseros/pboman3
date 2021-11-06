list(APPEND PROJECT_SOURCES
    "model/task/packtask.cpp"
    "model/task/packwindowmodel.cpp"
    "model/task/task.h"
    "model/task/taskwindowmodel.cpp"
    "model/task/unpacktask.cpp"
    "model/task/unpackwindowmodel.cpp"
    "model/conflictsparcel.cpp"
    "model/diskaccessexception.cpp"
    "model/headersmodel.cpp"
    "model/interactionparcel.cpp"
    "model/pbofileformatexception.cpp"
    "model/pbomodel.cpp"
    "model/rootreader.cpp"
    "model/signaturemodel.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "model/__test__/conflictsparcel_test.cpp"
    "model/__test__/headersmodel_test.cpp"
    "model/__test__/interactionparcel_test.cpp"
    "model/__test__/rootreader_test.cpp"
    "model/__test__/signaturemodel_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
