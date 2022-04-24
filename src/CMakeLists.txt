add_subdirectory(controller)

add_executable(manager manager.cpp)

target_link_libraries(manager
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        $<TARGET_OBJECTS:controller-ipc>
        Boost::thread
        Boost::program_options
        fmt::fmt
        mqttc
)

# needs c++ 14
target_compile_features(manager PRIVATE cxx_std_14)

install(TARGETS manager RUNTIME)
