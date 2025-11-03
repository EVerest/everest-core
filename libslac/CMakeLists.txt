cmake_minimum_required(VERSION 3.15.7)

include(FetchContent)

project(slac VERSION 0.1
        DESCRIPTION "Simple ISO15118-3 SLAC implementation"
		LANGUAGES CXX C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

add_subdirectory(3rd_party)

option(BUILD_SLAC_TOOLS "Build SLAC tools" OFF)
if (BUILD_SLAC_TOOLS)
    add_subdirectory(tools)
endif()

add_library(slac STATIC
    src/channel.cpp
    src/slac.cpp
    src/packet_socket.cpp
    $<TARGET_OBJECTS:HashLibrary>
)

target_include_directories(slac PRIVATE $<TARGET_PROPERTY:HashLibrary,INTERFACE_INCLUDE_DIRECTORIES>)

add_library(slac::slac ALIAS slac)


target_include_directories(slac PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS slac EXPORT SlacTargets
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ TYPE INCLUDE)

install(EXPORT SlacTargets
    FILE SlacTargets.cmake
    NAMESPACE slac::
    DESTINATION lib/cmake/slac
)


include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/SlacConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/slac"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SlacConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SlacConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SlacConfigVersion.cmake
    DESTINATION lib/cmake/slac
)

export(EXPORT SlacTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/SlacTargets.cmake"
)
