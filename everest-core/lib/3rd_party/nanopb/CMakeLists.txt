add_library(nanopb STATIC)
add_library(everest::nanopb ALIAS nanopb)

target_sources(nanopb
    PRIVATE
        nanopb/pb_common.c
        nanopb/pb_decode.c
        nanopb/pb_encode.c
)

# copy public header files for correct include prefix

file(COPY
        nanopb/pb.h
        nanopb/pb_encode.h
        nanopb/pb_decode.h
    DESTINATION
        ${THIRDPARTY_INCLUDE_TMP_DIR}/nanopb
)


target_include_directories(nanopb
    PUBLIC
        $<BUILD_INTERFACE:${EVEREST_CORE_LIB_INCLUDE_DIR}>
)

set_target_properties(nanopb PROPERTIES OUTPUT_NAME "everest_nanopb")

install(TARGETS nanopb
    EXPORT everest-core-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY
        ${THIRDPARTY_INCLUDE_TMP_DIR}/nanopb
    DESTINATION
        ${THIRDPARTY_INCLUDE_INSTALL_DIR}/
)
