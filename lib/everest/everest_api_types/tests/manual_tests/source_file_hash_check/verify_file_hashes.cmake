if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(Yellow      "${Esc}[33m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldCyan    "${Esc}[1;36m")
endif()

function(verify_hashes_at_configure_time expected_hashes_csv actual_hashes_csv)
    file(STRINGS "${expected_hashes_csv}" EXPECTED_LINES_RAW)
    file(STRINGS "${actual_hashes_csv}" ACTUAL_LINES_RAW)

    set(EXPECTED_LINES_CLEAN "")
    set(ACTUAL_LINES_CLEAN "")

    foreach(LINE ${EXPECTED_LINES_RAW})
        string(STRIP "${LINE}" LINE)
        if(NOT LINE STREQUAL "" AND NOT LINE MATCHES "^#")
            list(APPEND EXPECTED_LINES_CLEAN "${LINE}")
        endif()
    endforeach()

    foreach(LINE ${ACTUAL_LINES_RAW})
        string(STRIP "${LINE}" LINE)
        if(NOT LINE STREQUAL "" AND NOT LINE MATCHES "^#")
            list(APPEND ACTUAL_LINES_CLEAN "${LINE}")
        endif()
    endforeach()

    set(MISMATCH_LINES "")
# More recent CMake versions (>=3.17) can use "ZIP_LIST"
#    foreach(EXPECTED_LINE ACTUAL_LINE IN ZIP_LISTS ${EXPECTED_LINES_CLEAN} ${ACTUAL_LINES_CLEAN})
    list(LENGTH EXPECTED_LINES_CLEAN LEN_EXPECTED)
    math(EXPR LEN_EXPECTED_MINUS_ONE "${LEN_EXPECTED} - 1")

    foreach(i_count RANGE ${LEN_EXPECTED_MINUS_ONE})
        list(GET EXPECTED_LINES_CLEAN ${i_count} EXPECTED_LINE)
        list(GET ACTUAL_LINES_CLEAN ${i_count} ACTUAL_LINE)
# END: replacable by ZIP_LIST-Version

        if(NOT EXPECTED_LINE STREQUAL ACTUAL_LINE)
            string(APPEND MISMATCH_LINES "   ${ACTUAL_LINE} - ${EXPECTED_LINE}\n")
            set(FOUND_MISMATCH true BOOL)
        endif()
    endforeach()

    if(FOUND_MISMATCH)
        message(WARNING "${BoldYellow}\n"
"################################################################################\n"
"                           !!! HASH MISMATCH DETECTED !!!                       \n"
"${ColourReset}\n"
"   Unit tests are going to FAIL!                                                \n"
"                                                                                \n"
"   One or more file hashes do not match the values in                           \n"
"   '${expected_hashes_csv}':                                                    \n"
"${BoldYellow}\n"
"${MISMATCH_LINES}"
"${ColourReset}\n"
"   Please check your file contents or update the expected hashes.               \n"
"   Find a more detailed explanation in ${BoldCyan}lib/everest/everest_api_types/README.md  ${BoldYellow}\n"
"################################################################################\n"
"${ColourReset}")
    endif()
endfunction()

verify_hashes_at_configure_time(${EXPECTED_CSV} ${ACTUAL_CSV})