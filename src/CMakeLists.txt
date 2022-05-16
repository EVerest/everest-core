add_executable(charge_point charge_point.cpp)

target_link_libraries(charge_point
    PRIVATE
        ocpp ${CMAKE_DL_LIBS}
        Boost::thread
        Boost::program_options
        date::date
        date::date-tz
        everest::log
        sqlite3
)

# needs c++ 14
target_compile_features(charge_point PRIVATE cxx_std_14)

install(TARGETS  charge_point
        RUNTIME)
