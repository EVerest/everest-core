find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(everest-config-api-cli
    src/main.cpp
    src/mqtt_config_service_client.cpp
    src/yaml_provider.cpp
    src/command_handlers.cpp
)

target_include_directories(everest-config-api-cli PRIVATE src)
target_compile_options(everest-config-api-cli PRIVATE -Wall -Werror -Wextra)
target_compile_features(everest-config-api-cli PRIVATE cxx_std_17)

target_link_libraries(everest-config-api-cli
    PRIVATE
        everest::framework
        everest::everest_api_types
        everest::io
        Boost::program_options
)

install(
    TARGETS everest-config-api-cli
    RUNTIME
)