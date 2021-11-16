find_package(Threads REQUIRED)

add_executable(light_switch_fsm main.cpp fsm.cpp)

target_compile_features(light_switch_fsm PRIVATE cxx_std_14)

target_link_libraries(light_switch_fsm PRIVATE
    Fsm::Fsm
    Threads::Threads
)

# build the freertos demo

if(BUILD_EXAMPLES_WITH_FREERTOS_SUPPORT)
    add_executable(light_switch_fsm_freertos main_freertos.cpp fsm.cpp)

    target_compile_features(light_switch_fsm_freertos PRIVATE cxx_std_14)

    target_link_libraries(light_switch_fsm_freertos PRIVATE
        Fsm::Fsm
        freertos
        Threads::Threads
    )
endif()
