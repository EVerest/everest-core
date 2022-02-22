add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE
        everest::log
        Boost::program_options
)
target_compile_features(main PRIVATE cxx_std_14)

add_executable(test_trace test_trace.cpp)
target_link_libraries(test_trace PRIVATE
    everest::log
)
target_compile_features(test_trace PRIVATE cxx_std_14)

install(TARGETS main test_trace
        RUNTIME)

install(FILES logging.ini
        DESTINATION ".")
