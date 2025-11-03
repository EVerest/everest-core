set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/everest/timer.hpp"
)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# EVerest framework
add_library(timer INTERFACE)
add_library(everest::timer ALIAS timer)

target_include_directories(timer INTERFACE ${INCLUDE_DIR})
target_link_libraries(timer
INTERFACE
        Boost::system
)

# needs c++ 14
target_compile_features(timer INTERFACE cxx_std_14)

source_group(
    TREE "${INCLUDE_DIR}"
    PREFIX "Headers"
    FILES ${HEADERS})
