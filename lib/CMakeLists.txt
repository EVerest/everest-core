set(HEADERS
    "${PROJECT_SOURCE_DIR}/include/utils/config.hpp"
    "${PROJECT_SOURCE_DIR}/include/utils/exceptions.hpp"
    "${PROJECT_SOURCE_DIR}/include/utils/logging.hpp"
    "${PROJECT_SOURCE_DIR}/include/utils/message_queue.hpp"
    "${PROJECT_SOURCE_DIR}/include/utils/mqtt_abstraction.hpp"
    "${PROJECT_SOURCE_DIR}/include/utils/types.hpp"
	"${PROJECT_SOURCE_DIR}/include/utils/conversions.hpp"
    "${PROJECT_SOURCE_DIR}/include/framework/everest.hpp"
)

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

# EVerest framework
add_library(everest SHARED)
target_sources(everest
    PRIVATE
        config.cpp
        everest.cpp
        message_queue.cpp
        mqtt_abstraction.cpp
        mqtt_abstraction_impl.cpp
        conversions.cpp
        thread.cpp
)
target_include_directories(everest PUBLIC ${INCLUDE_DIR})
target_link_libraries(everest
    PRIVATE
        Boost::filesystem
        Boost::system
        Boost::thread
        date
        everest::log
        fmt::fmt
        mqttc
        nlohmann_json::nlohmann_json
        nlohmann_json_schema_validator::nlohmann_json_schema_validator
        Pal::Sigslot
)

# this is important to allow our headers to find the nlohmann_* headers
target_link_libraries(everest
    INTERFACE
        nlohmann_json_schema_validator::nlohmann_json_schema_validator)

# needs c++ 14
target_compile_features(everest PUBLIC cxx_std_14)

source_group(
    TREE "${INCLUDE_DIR}"
    PREFIX "Headers"
    FILES ${HEADERS})

install(TARGETS everest
    LIBRARY
    DESTINATION lib
)
