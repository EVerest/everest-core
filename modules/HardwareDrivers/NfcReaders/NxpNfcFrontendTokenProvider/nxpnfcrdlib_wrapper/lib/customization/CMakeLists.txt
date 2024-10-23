cmake_minimum_required(VERSION 3.15)

set(CUSTOM_DAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Platform/DAL)
set(CUSTOM_NXPLIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/NxpNfcRdLib)

add_subdirectory(Platform/DAL)
add_subdirectory(RTOS/phOsal)
add_subdirectory(NxpNfcRdLib)

add_library(NxpRdLib INTERFACE)
target_link_libraries(NxpRdLib
    INTERFACE
        phOsal
        DAL
        NxpRdLib_phNfcLib
        NxpRdLib_acDiscLoop
)
