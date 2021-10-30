list(APPEND PROJECT_SOURCES
    "io/bb/binarybackend.cpp"
    "io/bb/execbackend.cpp"
    "io/bb/nodefilesystem.cpp"
    "io/bb/sanitizedstring.cpp"
    "io/bb/tempbackend.cpp"
    "io/bb/unpackbackend.cpp"
    "io/bb/unpacktaskbackend.cpp"
    "io/bs/binarysource.cpp"
    "io/bs/fslzhbinarysource.cpp"
    "io/bs/fsrawbinarysource.cpp"
    "io/bs/pbobinarysource.cpp"
    "io/lzh/compressionbuffer.cpp"
    "io/lzh/compressionchunk.cpp"
    "io/lzh/decompressioncontext.cpp"
    "io/lzh/lzh.cpp"
    "io/lzh/lzhdecompressionexception.cpp"
    "io/pbodatastream.cpp"
    "io/pbofile.cpp"
    "io/pboheaderio.cpp"
    "io/pboheaderreader.cpp"
    "io/pboioexception.cpp"
    "io/pbowriter.cpp")

set(PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)

list(APPEND TEST_SOURCES
    "io/bb/__test__/execbackend_test.cpp"
    "io/bb/__test__/nodefilesystem_test.cpp"
    "io/bb/__test__/sanitizedstring_test.cpp"
    "io/bb/__test__/tempbackend_test.cpp"
    "io/bb/__test__/unpackbackend_test.cpp"
    "io/bs/__test__/fslzhbinarysource_test.cpp"
    "io/bs/__test__/fsrawbinarysource_test.cpp"
    "io/bs/__test__/pbobinarysource_test.cpp"
    "io/lzh/__test__/compressionbuffer_test.cpp"
    "io/lzh/__test__/compressionchunk_test.cpp"
    "io/lzh/__test__/lzh_test.cpp"
    "io/__test__/pbofile_test.cpp"
    "io/__test__/pboheaderio_test.cpp"
    "io/__test__/pboheaderreader_test.cpp"
    "io/__test__/pbowriter_test.cpp")

set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
