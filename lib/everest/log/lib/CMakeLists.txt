add_library(everest_log)
add_library(everest::log ALIAS everest_log)

target_sources(everest_log
    PRIVATE
        logging.cpp
        trace.cpp
)

target_include_directories(everest_log
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(everest_log
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(everest_log
    PUBLIC
        Boost::log
    PRIVATE
        Boost::log_setup
)

if (LIBLOG_USE_BOOST_FILESYSTEM)
    target_link_libraries(everest_log
        PRIVATE
            Boost::filesystem
    )
    target_compile_definitions(everest_log
        PRIVATE
            LIBLOG_USE_BOOST_FILESYSTEM
    )
endif()

# FIXME (aw): in case FindBoost.cmake was used we need to add things
#             this should be removed no support for Boost < 1.74 is needed
if (NOT Boost_DIR)
    target_compile_definitions(everest_log
        PUBLIC
            BOOST_LOG_DYN_LINK
    )
    target_link_libraries(everest_log
        PUBLIC
            rt dl
    )
endif()

if (BUILD_BACKTRACE_SUPPORT)
    target_link_libraries(everest_log
        PRIVATE
            backtrace
    )
    target_compile_definitions(everest_log PRIVATE WITH_LIBBACKTRACE)
endif()

target_compile_features(everest_log PUBLIC cxx_std_17)
