find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(evse)

add_executable(bridge bridge.cpp)
target_link_libraries(bridge PRIVATE
    Threads::Threads
    fmt::fmt
    slac::slac
)
