add_library(controller-ipc OBJECT ipc.cpp)

target_link_libraries(controller-ipc
    PUBLIC
        nlohmann_json::nlohmann_json
)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)

add_executable(controller 
    controller.cpp
    command_api.cpp
    rpc.cpp
    server.cpp
    $<TARGET_OBJECTS:controller-ipc>
)

target_include_directories(controller PRIVATE
    ${websocketpp_SOURCE_DIR}
)

target_link_libraries(controller PRIVATE
    Threads::Threads
    nlohmann_json::nlohmann_json
    ${Boost_FILESYSTEM_LIBRARY}
    fmt::fmt
    everest::log
)

target_compile_features(controller PRIVATE cxx_std_14)

install(TARGETS controller RUNTIME)
