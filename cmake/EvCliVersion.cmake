# check ev-cli version

function(get_ev_cli_version EV_CLI_VERSION_IN OUTPUT_EV_CLI_VERSION)
    string(REPLACE "ev-cli " "" EV_CLI_VERSION "${EV_CLI_VERSION_IN}")
    set(${OUTPUT_EV_CLI_VERSION} "${EV_CLI_VERSION}" PARENT_SCOPE)
endfunction()

function(require_ev_cli_version EV_CLI_VERSION_IN EV_CLI_VERSION_REQUIRED)
    get_ev_cli_version("${EV_CLI_VERSION_IN}" EV_CLI_VERSION)
    if ("${EV_CLI_VERSION}" STREQUAL "")
        message(FATAL_ERROR "Could not determine a ev-cli version from the provided version '${EV_CLI_VERSION_IN}'")
    endif()
    if("${EV_CLI_VERSION}" VERSION_GREATER_EQUAL "${EV_CLI_VERSION_REQUIRED}")
        message("Found ev-cli version '${EV_CLI_VERSION}' which satisfies the requirement of ev-cli version '${EV_CLI_VERSION_REQUIRED}'")
    else()
        message(FATAL_ERROR "ev-cli version ${EV_CLI_VERSION_REQUIRED} or higher is required. However your ev-cli version is '${EV_CLI_VERSION}'. Please upgrade ev-cli.")
    endif()
endfunction()
