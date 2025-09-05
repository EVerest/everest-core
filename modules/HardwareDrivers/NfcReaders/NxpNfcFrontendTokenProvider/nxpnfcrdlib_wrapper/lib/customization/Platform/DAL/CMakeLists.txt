cmake_minimum_required(VERSION 3.15)

project(DAL)

add_library(DAL)
target_sources(DAL
    PRIVATE
        ${DAL_PATH}/src/Linux/phDriver_Linux_Int.c
        ${DAL_PATH}/src/Linux/phbalReg_Linux_UserSpi.c
        ${DAL_PATH}/src/Linux/phDriver_Linux.c
        # Add c files in src to provide custom implementation of the GPIO and register access (SPI/I²C) interface
        # Remove the files given above individually, if a replacement is added below.
        # Individual replacements:
        src/rpi4_gpio.c
)
target_include_directories(DAL
    PUBLIC
        $<BUILD_INTERFACE:${DAL_PATH}/src/Linux>
        $<BUILD_INTERFACE:${DAL_PATH}/inc>
        $<BUILD_INTERFACE:${DAL_PATH}/boards>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/boards>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cfg>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/inc>
)
target_compile_definitions(DAL
    PUBLIC
        PHDRIVER_PI4RC663_BOARD
)
