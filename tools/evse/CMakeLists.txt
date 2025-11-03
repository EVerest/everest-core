find_package(Threads REQUIRED)

FetchContent_Declare(
    libfsm
    GIT_REPOSITORY https://github.com/EVerest/libfsm
    GIT_TAG refactor/sync_fsm
)

FetchContent_MakeAvailable(libfsm)

add_executable(evse evse.cpp evse_fsm.cpp slac_io.cpp)

target_compile_features(evse PRIVATE cxx_std_14)

target_link_libraries(evse PRIVATE
    fsm::fsm
    slac::slac
    Threads::Threads
    fmt::fmt
)
