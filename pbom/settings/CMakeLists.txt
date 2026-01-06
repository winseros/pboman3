list(APPEND PROJECT_SOURCES
        "settings/applicationsettingsmanager.h"
        "settings/getapplicationsettingsmanager.cpp"
        "settings/localstorageapplicationsettingsmanager.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
        "settings/__test__/localstorageapplicationsettingsmanager_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)