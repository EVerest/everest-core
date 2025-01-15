add_library(gpio STATIC)
add_library(everest::gpio ALIAS gpio)

target_sources(gpio
    PRIVATE
    gpio.cpp
)

target_include_directories(gpio
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(gpio
    PRIVATE
)

ev_register_library_target(gpio)

