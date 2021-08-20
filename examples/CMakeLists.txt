add_executable(main main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
target_link_libraries(main
    PRIVATE
        date
        everest::timer
        Boost::program_options
        Threads::Threads
)

# needs c++ 14
target_compile_features(main PRIVATE cxx_std_14)

install(TARGETS main
        RUNTIME)
