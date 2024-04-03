# Licensor: Pionix GmbH, 2024
# License: BaseCamp - License Version 1.0
#
# Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
# under: https://pionix.com/pionix-license-terms
# You may not use this file/code except in compliance with said License.

# where to place generated .cpp and .h files
set(NANOPB_OUTDIR ${CMAKE_BINARY_DIR}/generated/protobuf)
file(MAKE_DIRECTORY ${NANOPB_OUTDIR})
# location of python scripts
set(NANOPB_SCRIPT_D ${CMAKE_CURRENT_LIST_DIR}/scripts)

# ${Python3_EXECUTABLE} is not used because nanopb isn't installed when using a
# virtual environment. Always use python from the path

# convert a .proto file into .cpp and .h files
function(generate_nanopb PROTO)
    message("Adding nanopb on: ${PROTO}")
    cmake_policy(SET CMP0116 NEW)
    add_custom_command(
        OUTPUT ${NANOPB_OUTDIR}/${PROTO}.pb.cpp ${NANOPB_OUTDIR}/${PROTO}.pb.h
        DEPFILE ${NANOPB_OUTDIR}/${PROTO}.pb.cpp.d
        COMMAND python3 ${NANOPB_SCRIPT_D}/generate_dependencies.py -I ${CMAKE_CURRENT_SOURCE_DIR} ${PROTO}.proto ${NANOPB_OUTDIR}/${PROTO}.pb.cpp.d
        COMMAND python3 ${NANOPB_SCRIPT_D}/basecamp_nanopb.py -D ${NANOPB_OUTDIR} -S .cpp -s mangle_names:M_FLATTEN -I ${CMAKE_CURRENT_SOURCE_DIR} ${PROTO}.proto
        DEPENDS ${CHECK_DONE_FILE} ${NANOPB_SCRIPT_D}/basecamp_nanopb.py ${NANOPB_SCRIPT_D}/generate_dependencies.py ${PROTO}.proto
    )
endfunction()
