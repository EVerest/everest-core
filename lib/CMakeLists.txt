add_library(log)
add_library(everest::log ALIAS log)

target_sources(log
    PRIVATE
        logging.cpp
        trace.cpp
)

target_include_directories(log
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(log
    PUBLIC
        Boost::log
    PRIVATE
        Boost::log_setup
)

if (LIBLOG_USE_BOOST_FILESYSTEM)
    target_link_libraries(log
        PRIVATE
            Boost::filesystem
    )
    target_compile_definitions(log
        PRIVATE
            LIBLOG_USE_BOOST_FILESYSTEM
    )
endif()

# FIXME (aw): in case FindBoost.cmake was used we need to add things
#             this should be removed no support for Boost < 1.74 is needed
if (NOT Boost_DIR)
    target_compile_definitions(log
        PUBLIC
            BOOST_LOG_DYN_LINK
    )
    target_link_libraries(log
        PUBLIC
            rt dl
    )
endif()

if (BUILD_BACKTRACE_SUPPORT)
    target_link_libraries(log
        PRIVATE
            backtrace
    )
    target_compile_definitions(log PRIVATE WITH_LIBBACKTRACE)
endif()
