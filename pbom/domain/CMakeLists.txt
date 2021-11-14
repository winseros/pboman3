list(APPEND PROJECT_SOURCES
    "domain/binarysource.cpp"
    "domain/documentheader.cpp"
    "domain/documentheaders.cpp"
    "domain/documentheaderstransaction.cpp"
    "domain/pbodocument.cpp"
    "domain/pbonode.cpp"
    "domain/pbonodetransaction.cpp"
    "domain/pbopath.cpp"
    "domain/validationexception.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "domain/__test__/documentheader_test.cpp"
    "domain/__test__/documentheaders_test.cpp"
    "domain/__test__/documentheaderstransaction_test.cpp"
    "domain/__test__/pbodocument_test.cpp"
    "domain/__test__/pbonode_test.cpp"
    "domain/__test__/pbonodetransaction_test.cpp"
    "domain/__test__/pbopath_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
