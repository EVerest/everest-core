add_library(watchdog STATIC)
add_library(everest::watchdog ALIAS watchdog)

target_sources(watchdog
    PRIVATE
    watchdog.cpp
)

target_include_directories(watchdog
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(watchdog
    PUBLIC
    Pal::Sigslot
)

