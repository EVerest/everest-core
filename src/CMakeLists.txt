add_subdirectory(controller)

add_executable(manager manager.cpp)

target_link_libraries(manager
    PRIVATE
        everest::framework
        $<TARGET_OBJECTS:controller-ipc>
        Boost::program_options
)

# FIXME (aw): only necessary due to invalid conversion from
#             'const char*' to 'char*'
target_compile_features(manager PRIVATE cxx_std_17)

install(
    TARGETS manager
    RUNTIME
)
