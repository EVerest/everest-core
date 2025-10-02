cmake_minimum_required(VERSION 3.15)

project(phOsal)

find_package(Threads REQUIRED)

add_library(phOsal)
target_sources(phOsal
    PRIVATE
        ${PHOSAL_PATH}/src/Linux/phOsal_Linux.c
)
target_include_directories(phOsal
    PUBLIC
        $<BUILD_INTERFACE:${PHOSAL_PATH}/inc>
        $<BUILD_INTERFACE:${PHOSAL_PATH}/src/Linux>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(phOsal
    PRIVATE
        Threads::Threads -lrt
)
target_compile_definitions(phOsal
    PUBLIC
        PH_OSAL_LINUX
)
