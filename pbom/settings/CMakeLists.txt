list(APPEND PROJECT_SOURCES
        "settings/getsettingsfacility.cpp"
        "settings/localstorageapplicationsettingsfacility.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
        "settings/__test__/localstorageapplicationsettingsfacility_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)