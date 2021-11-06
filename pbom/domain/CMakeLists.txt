list(APPEND PROJECT_SOURCES
    "domain/binarysource.cpp"
    "domain/pbonode.cpp"
    "domain/pbopath.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "domain/__test__/pbonode_test.cpp"
    "domain/__test__/pbopath_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
