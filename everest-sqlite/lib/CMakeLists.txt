
add_library(everest_sqlite)
add_library(everest::sqlite ALIAS everest_sqlite)

target_sources(everest_sqlite
    PRIVATE
        everest/database/sqlite/statement.cpp
        everest/database/sqlite/connection.cpp
        everest/database/sqlite/schema_updater.cpp
)

target_link_libraries(everest_sqlite
    PRIVATE
        SQLite::SQLite3
        nlohmann_json::nlohmann_json
)

target_include_directories(everest_sqlite
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(everest_sqlite
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

#############
# Logging configuration
#############
if (EVEREST_CUSTOM_LOGGING_LIBRARY)
    if(NOT TARGET ${EVEREST_CUSTOM_LOGGING_LIBRARY})
        message(FATAL_ERROR "${EVEREST_CUSTOM_LOGGING_LIBRARY} is not a valid library")
    else()
        target_link_libraries(everest_sqlite
            PUBLIC
            ${EVEREST_CUSTOM_LOGGING_LIBRARY}
        )
        message(STATUS "Using custom logging library: ${EVEREST_CUSTOM_LOGGING_LIBRARY}")
    endif()
elseif (EVEREST_CUSTOM_LOGGING_INCLUDE_PATH)
    if (NOT EXISTS "${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
        message(FATAL_ERROR "everest/logging.hpp not found in directory ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}")
    else()
        target_include_directories(everest_sqlite
            PUBLIC
            include
            ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}
        )
    endif()
    message(STATUS "Using the following logging header: ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
endif()

if (NOT EVEREST_CUSTOM_LOGGING_INCLUDE_PATH)
    target_link_libraries(everest_sqlite
        PUBLIC
        everest::log
    )
    message(STATUS "Using the default logging header")
endif()

#############
# End logging configuration
#############

target_compile_features(everest_sqlite PRIVATE cxx_std_17)
