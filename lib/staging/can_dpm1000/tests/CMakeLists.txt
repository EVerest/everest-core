add_executable(dpm1000_tester)
target_sources(dpm1000_tester
    PRIVATE
        dpm1000_tester.cpp
)

target_link_libraries(dpm1000_tester
    PRIVATE
        can_protocols::dpm1000
        Threads::Threads
)
