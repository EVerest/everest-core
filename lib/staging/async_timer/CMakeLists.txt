add_library(async_timer INTERFACE)
add_library(everest::async_timer ALIAS async_timer)

target_include_directories(async_timer INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

