list(APPEND PROJECT_SOURCES
    "util/filenames.cpp"
    "util/json.cpp"
    "util/log.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "util/__test__/filenames_test.cpp"
    "util/__test__/json_test.cpp"
    "util/__test__/qpointerlistiterator_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
