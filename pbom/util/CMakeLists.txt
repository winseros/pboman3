list(APPEND PROJECT_SOURCES
    "util/log.cpp"
    "util/util.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "util/__test__/qpointerlistiterator_test.cpp"
    "util/__test__/util_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
