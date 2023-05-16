list(APPEND PROJECT_SOURCES
    "infrastructure/localstorageapplicationsettingsfacility.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "infrastructure/__test__/localstorageapplicationsettingsfacility_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
