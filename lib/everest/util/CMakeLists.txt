add_library(everest_util INTERFACE)
add_library(everest::util ALIAS everest_util)
ev_register_library_target(everest_helpers)

set_target_properties(everest_util PROPERTIES
  VERSION 0.0.1
)


target_include_directories(everest_util
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(everest_util
    PROPERTIES
        BUILD_INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/everest
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
