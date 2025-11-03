find_package(Threads REQUIRED)

add_subdirectory(evse)

add_executable(bridge bridge.cpp)
target_compile_features(bridge PRIVATE cxx_std_14)
target_link_libraries(bridge PRIVATE
    slac::slac
    Threads::Threads
    fmt::fmt
)
