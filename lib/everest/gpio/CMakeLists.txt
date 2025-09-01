add_library(gpio STATIC)
add_library(everest::gpio ALIAS gpio)
ev_register_library_target(gpio)

target_sources(gpio
    PRIVATE
    src/gpio.cpp
)

target_include_directories(gpio
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(gpio
    PRIVATE
)

install(TARGETS gpio
    EXPORT everest-core-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/everest
    FILES_MATCHING PATTERN "*.hpp"
)
