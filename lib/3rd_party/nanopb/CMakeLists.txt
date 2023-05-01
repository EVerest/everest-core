add_library(nanopb STATIC)
add_library(everest::nanopb ALIAS nanopb)

target_sources(nanopb
    PRIVATE
        nanopb/pb_common.c
        nanopb/pb_decode.c
        nanopb/pb_encode.c
)

target_include_directories(nanopb
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
