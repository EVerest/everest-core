
set(EVEREST_VERSION_INFORMATION_TXT_IN "${CMAKE_CURRENT_LIST_DIR}/assets/version_information.txt.in")

function (evc_generate_version_information)
    execute_process(
        COMMAND
            git describe --dirty --always --tags
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE
            GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # make version information available txt file
    configure_file("${EVEREST_VERSION_INFORMATION_TXT_IN}" "${CMAKE_CURRENT_BINARY_DIR}/generated/version_information.txt" @ONLY)
endfunction()
