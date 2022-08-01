set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/modules/Auth/include")

add_library(auth_handler STATIC)

target_sources(auth_handler PRIVATE
    AuthHandler.cpp
    Connector.cpp
    ReservationHandler.cpp
    ConnectorStateMachine.cpp
)

get_target_property(GENERATED_INCLUDE_DIR generate_cpp_files EVEREST_GENERATED_INCLUDE_DIR)

target_include_directories(auth_handler PRIVATE 
    ${INCLUDE_DIR}
    ${GENERATED_INCLUDE_DIR}
)

target_link_libraries(auth_handler
PUBLIC
    fsm::fsm
PRIVATE
    everest::log
    everest::timer
    date::date
    date::date-tz
    nlohmann_json::nlohmann_json
    everest::framework
)

# needs c++ 14
target_compile_features(auth_handler PRIVATE cxx_std_14)

