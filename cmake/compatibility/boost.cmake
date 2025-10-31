# compatibility for Boost >= 1.89 while keeping backwards compatibility with existing code
if(Boost_VERSION_STRING VERSION_LESS "1.69.0")
    # Boost.System is a header only library only from 1.69.0
    find_package(Boost
        COMPONENTS
            system
        REQUIRED
    )
endif()

if(Boost_VERSION_STRING VERSION_GREATER_EQUAL "1.89.0")
    # starting from Boost 1.89.0 the included compatibility layer is removed
    # here we re-introduce a very simplified version of it
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/compatibility/boost_systemConfig.cmake"
        ${CMAKE_BINARY_DIR}/boost_system/boost_systemConfig.cmake
        COPYONLY
    )
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/compatibility/boost_systemConfigVersion.cmake"
        ${CMAKE_BINARY_DIR}/boost_system/boost_systemConfigVersion.cmake
        COPYONLY
    )
    set(boost_system_DIR ${CMAKE_BINARY_DIR}/boost_system CACHE PATH "")
endif()
