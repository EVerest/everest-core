add_subdirectory(controller)

add_executable(manager manager.cpp)

target_link_libraries(manager
    PRIVATE
        everest::framework
        controller-ipc
        Boost::program_options
)

target_compile_options(manager PRIVATE ${COMPILER_WARNING_OPTIONS})

target_compile_features(manager PRIVATE cxx_std_17)

install(
    TARGETS manager
    RUNTIME
)
