find_package(Threads REQUIRED)

add_executable(ls_async_pthread
    ls_async_pthread.cpp
    async_fsm.cpp
)

target_link_libraries(ls_async_pthread
    PRIVATE
        fsm::fsm
        Threads::Threads
)

add_executable(ls_sync_posix
    ls_sync_posix.cpp
    sync_fsm.cpp
)

target_link_libraries(ls_sync_posix PRIVATE
    fsm::fsm
    Threads::Threads
)
