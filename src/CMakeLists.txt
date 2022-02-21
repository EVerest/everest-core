add_executable(main main.cpp)

target_link_libraries(main
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        Boost::thread
        Boost::program_options
        fmt::fmt
        mqttc
)

# needs c++ 14
target_compile_features(main PRIVATE cxx_std_14)

add_executable(manager manager.cpp)

target_link_libraries(manager
    PRIVATE
        everest ${CMAKE_DL_LIBS}
        everest::log
        Boost::thread
        Boost::program_options
        fmt::fmt
        mqttc
)

# needs c++ 14
target_compile_features(manager PRIVATE cxx_std_14)

install(TARGETS main manager
        RUNTIME)
