set(EVEREST_VERSION_INFORMATION_HEADER_IN "${CMAKE_CURRENT_LIST_DIR}/assets/version_information.hpp.in")
set(EVEREST_VERSION_INFORMATION_TXT_IN "${CMAKE_CURRENT_LIST_DIR}/assets/version_information.txt.in")

function (evc_generate_version_information)
    execute_process(
        COMMAND
            git describe --dirty --always --tags
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE
            GIT_DESCRIBE_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND
            git symbolic-ref --quiet --short HEAD
        WORKING_DIRECTORY
            ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE
            GIT_SYMBOLIC_REF_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(EVEREST_GIT_VERSION "${GIT_SYMBOLIC_REF_OUTPUT}@${GIT_DESCRIBE_OUTPUT}")
    # make version information available as c++ header and txt file
    configure_file("${EVEREST_VERSION_INFORMATION_HEADER_IN}" "${CMAKE_CURRENT_BINARY_DIR}/generated/include/generated/version_information.hpp" @ONLY)
    configure_file("${EVEREST_VERSION_INFORMATION_TXT_IN}" "${CMAKE_CURRENT_BINARY_DIR}/generated/version_information.txt" @ONLY)
endfunction()
