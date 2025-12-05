#
# options
#
option(BUILD_EXAMPLES "enable building of examples" OFF)


#
# library declaration
#
add_library(fsm INTERFACE)
add_library(fsm::fsm ALIAS fsm)

target_include_directories(fsm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

#
# examples
#
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#
# tests
#
if(BUILD_TESTING)
    include(CTest)
    add_subdirectory(tests)
endif()
