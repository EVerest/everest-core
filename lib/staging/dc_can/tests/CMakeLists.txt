add_executable(dc_tester)
target_sources(dc_tester
    PRIVATE
        main.cpp
)

target_link_libraries(dc_tester
    PRIVATE
        dc_can
        Threads::Threads
)

target_compile_features(dc_tester PRIVATE cxx_std_17)
