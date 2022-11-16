add_library(controller-ipc OBJECT ipc.cpp)

target_link_libraries(controller-ipc
    PUBLIC
        nlohmann_json::nlohmann_json
)

find_package(Threads REQUIRED)

# FIXME (aw): refactor the yaml_loader into on lib, so it can be shared somehow, similar to the controller-ipc
add_executable(controller 
    controller.cpp
    command_api.cpp
    rpc.cpp
    server.cpp
    ${PROJECT_SOURCE_DIR}/lib/yaml_loader.cpp
    $<TARGET_OBJECTS:controller-ipc>
)

target_include_directories(controller PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${websocketpp_SOURCE_DIR}
)

target_link_libraries(controller
    PRIVATE
        Threads::Threads

        nlohmann_json::nlohmann_json

        websocketpp::websocketpp
        fmt::fmt

        everest::log

        ryml::ryml
)

target_compile_features(controller PRIVATE cxx_std_17)

install(TARGETS controller RUNTIME)
