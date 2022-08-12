find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE
        everest::log
        Boost::program_options
)

add_executable(test_trace test_trace.cpp)
target_link_libraries(test_trace PRIVATE
    everest::log
)
