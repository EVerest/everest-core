add_library(test_utilities OBJECT codec.cpp)
target_link_libraries(test_utilities
    PUBLIC
        cbv2g::din
        cbv2g::iso2
        cbv2g::iso20
)

target_include_directories(test_utilities
    PUBLIC
        include
)

function(add_codec_test CPP_FILE)
    set(TEST_TARGET_NAME test_${CPP_FILE})
    add_executable(${TEST_TARGET_NAME} ${CPP_FILE}.cpp)

    target_link_libraries(${TEST_TARGET_NAME}
        PRIVATE            
            test_utilities
            Catch2::Catch2WithMain
    )

    catch_discover_tests(${TEST_TARGET_NAME})
endfunction()
