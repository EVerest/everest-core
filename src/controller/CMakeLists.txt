add_library(controller-ipc OBJECT ipc.cpp)

target_link_libraries(controller-ipc
    PUBLIC
        nlohmann_json::nlohmann_json
)

target_compile_options(controller-ipc PRIVATE ${COMPILER_WARNING_OPTIONS})

find_package(Threads REQUIRED)

# FIXME (aw): refactor the yaml_loader into on lib, so it can be shared somehow, similar to the controller-ipc
add_executable(controller 
    controller.cpp
    command_api.cpp
    rpc.cpp
    server.cpp
    ${PROJECT_SOURCE_DIR}/lib/yaml_loader.cpp
)

target_include_directories(controller PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(controller PRIVATE ${COMPILER_WARNING_OPTIONS})

target_link_libraries(controller
    PRIVATE
        Threads::Threads

        controller-ipc

        websockets_shared
        fmt::fmt

        everest::log

        ryml::ryml
)

target_compile_features(controller PRIVATE cxx_std_17)

install(TARGETS controller RUNTIME)

if (EVEREST_INSTALL_ADMIN_PANEL)
    include(FetchContent)

    message(STATUS "Adding admin-panel")

    FetchContent_Declare(admin-panel
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/EVerest/everest-admin-panel/releases/download/v0.1.0/everest-admin-panel.tar.gz
    )

    FetchContent_MakeAvailable(admin-panel)
    install(
        DIRECTORY ${admin-panel_SOURCE_DIR}/
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/everest/www"
    )
endif ()
