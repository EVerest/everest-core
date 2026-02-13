find_package(Threads REQUIRED)

add_library(ieee2030)
add_library(ieee2030::ieee2030 ALIAS ieee2030)

target_sources(ieee2030
    PRIVATE

    # Common
    common/io/logging.cpp
    common/io/time.cpp
    common/messages/messages.cpp

    # Charger
    charger/charger_controller.cpp

    charger/session/callback.cpp
    charger/session/session.cpp

    charger/v20/context.cpp

    charger/v20/state/state_b.cpp
    charger/v20/state/state_c.cpp

    charger/io/can_broker_charger.cpp

    # EV
    ev/io/can_broker_ev.cpp
)

target_include_directories(ieee2030
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(ieee2030
    PRIVATE
        Threads::Threads
)

target_compile_features(ieee2030 PUBLIC cxx_std_17)
