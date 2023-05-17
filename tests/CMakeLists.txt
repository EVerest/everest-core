include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.1
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)


if (CODE_COVERAGE)
    evc_include(CodeCoverage)
endif()

add_executable(test_state_allocator state_allocator.cpp)
target_link_libraries(test_state_allocator
    PRIVATE
        fsm::fsm
        Catch2::Catch2WithMain
)

if (CODE_COVERAGE)
    append_coverage_compiler_flags_to_target(test_state_allocator)
    setup_target_for_coverage_gcovr_html(
        NAME coverage
        EXECUTABLE test_state_allocator
        BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/include"
)
endif()

catch_discover_tests(test_state_allocator)
