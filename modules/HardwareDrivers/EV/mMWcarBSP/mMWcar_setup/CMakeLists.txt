cmake_minimum_required(VERSION 3.10)

# set the project name
project(mMWcar_setup VERSION 0.1)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Threads REQUIRED)

# add ftxui
find_package(ftxui 5 QUIET)
if (NOT ftxui_FOUND)
    message(STATUS "Retreiving ftxui using FetchContent")
    include(FetchContent)

    FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v5.0.0
    )
    FetchContent_MakeAvailable(ftxui)
endif()

# add the executable
add_executable(mMWcar_setup main.cpp)
target_include_directories(mMWcar_setup
    PUBLIC
    "${PROJECT_BINARY_DIR}"
    "../mMWcar_mcu_comms/protobuf"
    "../mMWcar_mcu_comms"
)

target_link_libraries(mMWcar_setup
    PRIVATE
        Pal::Sigslot
        Threads::Threads
        mMWcar_mcu_comms
        everest::framework
        everest::nanopb
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

install(TARGETS mMWcar_setup
        DESTINATION ${EVEREST_MOD_MMWCARBSP_DESTINATION})