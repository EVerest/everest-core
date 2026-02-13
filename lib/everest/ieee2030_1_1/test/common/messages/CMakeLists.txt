include(Catch)

add_executable(test_charger_messages charger_messages.cpp)

target_link_libraries(test_charger_messages
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_charger_messages)

add_executable(test_ev_messages ev_messages.cpp)

target_link_libraries(test_ev_messages
    PRIVATE
        ieee2030::ieee2030
        Catch2::Catch2WithMain
)

catch_discover_tests(test_ev_messages)
