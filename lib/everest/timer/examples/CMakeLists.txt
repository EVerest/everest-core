set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

find_package(Boost COMPONENTS program_options REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE
        Threads::Threads
        Boost::program_options
        everest::timer
)
