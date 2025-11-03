set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/everest/exceptions.hpp"
    "${PROJECT_SOURCE_DIR}/include/everest/logging.hpp"
)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# EVerest framework
add_library(log STATIC)
add_library(everest::log ALIAS log)

target_sources(log
    PRIVATE
        logging.cpp
)

target_include_directories(log PUBLIC ${INCLUDE_DIR})
target_link_libraries(log
    PUBLIC
        Boost::filesystem
        Boost::log_setup
        Boost::log
)

# needs c++ 14
target_compile_features(log PUBLIC cxx_std_14)

source_group(
    TREE "${INCLUDE_DIR}"
    PREFIX "Headers"
    FILES ${HEADERS})
