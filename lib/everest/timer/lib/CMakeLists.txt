add_library(timer INTERFACE)
add_library(everest::timer ALIAS timer)

target_include_directories(timer
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

if(Boost_VERSION_STRING VERSION_LESS "1.69.0")
    set(EVEREST_LIBTIMER_BOOST_SYSTEM_LINK_LIBRARY Boost::system)
endif()

target_link_libraries(timer
    INTERFACE
        ${EVEREST_LIBTIMER_BOOST_SYSTEM_LINK_LIBRARY}
        date::date-tz
)
