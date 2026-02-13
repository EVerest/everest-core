include(Catch)

add_executable(can_charger)

target_sources(can_charger
    PRIVATE
        can_charger.cpp
)

target_link_libraries(can_charger
    PRIVATE
        ieee2030::ieee2030
)
