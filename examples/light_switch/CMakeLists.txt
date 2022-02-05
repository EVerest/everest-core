find_package(Threads REQUIRED)

add_executable(ls_async_pthread ls_async_pthread.cpp async_fsm.cpp)

target_compile_features(ls_async_pthread PRIVATE cxx_std_14)

target_link_libraries(ls_async_pthread PRIVATE
    fsm::fsm
    Threads::Threads
)

add_executable(ls_sync_posix ls_sync_posix.cpp sync_fsm.cpp)

target_compile_features(ls_sync_posix PRIVATE cxx_std_14)

target_link_libraries(ls_sync_posix PRIVATE
    fsm::fsm
    Threads::Threads
)

# build the freertos demo

if(BUILD_EXAMPLES_WITH_FREERTOS_SUPPORT)
    add_executable(ls_async_freertos ls_async_freertos.cpp async_fsm.cpp)

    target_compile_features(ls_async_freertos PRIVATE cxx_std_14)

    target_link_libraries(ls_async_freertos PRIVATE
        fsm::fsm
        freertos
        Threads::Threads
    )
endif()
