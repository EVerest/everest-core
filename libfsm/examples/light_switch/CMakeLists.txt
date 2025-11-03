add_executable(light_switch)

target_sources(light_switch
    PRIVATE
        light_switch.cpp
        states.cpp
        context.cpp
)

target_link_libraries(light_switch
    PRIVATE
        fsm::fsm
)

# target_compile_definitions(light_switch PUBLIC HEAP_FREE_MODE)
target_compile_features(light_switch PRIVATE cxx_std_14)
