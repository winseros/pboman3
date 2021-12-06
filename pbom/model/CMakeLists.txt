list(APPEND PROJECT_SOURCES
    "model/task/packconfiguration.cpp"
    "model/task/packoptions.cpp"
    "model/task/packtask.cpp"
    "model/task/packwindowmodel.cpp"
    "model/task/task.h"
    "model/task/taskwindowmodel.cpp"
    "model/task/unpacktask.cpp"
    "model/task/unpackwindowmodel.cpp"
    "model/conflictsparcel.cpp"
    "model/interactionparcel.cpp"
    "model/pbomodel.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "model/task/__test__/packconfiguration_test.cpp"
    "model/task/__test__/packoptions_test.cpp"
    "model/__test__/conflictsparcel_test.cpp"
    "model/__test__/interactionparcel_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
