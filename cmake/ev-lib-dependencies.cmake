#
# Selective library inclusion for everest-core.
#
# When EVEREST_INCLUDE_LIBS is set (allowlist mode), only listed libraries
# and their transitive internal dependencies are built.
# When EVEREST_EXCLUDE_LIBS is set (blocklist mode), listed libraries are skipped.
# When neither is set, all libraries are built (default / backwards-compatible).
#
# Usage from an external project via EDM/CPM:
#   options:
#     - "EVEREST_LIBS_ONLY ON"
#     - "EVEREST_INCLUDE_LIBS log;util;io"
#
# Adding a new library:
#   1. Create lib/everest/<name>/ with its CMakeLists.txt
#   2. Add <name> to EVEREST_LIB_SUBDIRS below
#   3. If it depends on other everest-core libraries, add:
#      set(EVEREST_LIB_DEPS_<name> "dep1;dep2")
#

# --- Master list of library subdirectories ---
# This is the single source of truth for which libraries exist under lib/everest/.
# Used by both the build-variable computation here and lib/everest/CMakeLists.txt.
set(EVEREST_LIB_SUBDIRS
    can_dpm1000
    cbv2g
    conversions
    crc
    everest_api_types
    evse_security
    external_energy_limits
    framework
    fsm
    helpers
    ieee2030_1_1
    io
    iso15118
    log
    ocpp
    run_application
    slac
    sqlite
    timer
    tls
    util
    yaml
)

# --- Internal dependency map ---
# Each variable lists the everest-core libraries that the key library depends on.
# Only internal (lib/everest/) dependencies are tracked here; external deps
# (Boost, OpenSSL, etc.) are handled by each library's own CMakeLists.txt.
# Libraries with no internal deps do not need an entry.

# Tier 1
set(EVEREST_LIB_DEPS_yaml "log")
set(EVEREST_LIB_DEPS_io "util")
set(EVEREST_LIB_DEPS_run_application "log")
set(EVEREST_LIB_DEPS_evse_security "cbv2g")
# Tier 2
set(EVEREST_LIB_DEPS_ocpp "log;timer;evse_security;sqlite")
set(EVEREST_LIB_DEPS_iso15118 "cbv2g")
set(EVEREST_LIB_DEPS_ieee2030_1_1 "framework")
# Tier 3 (framework-coupled)
set(EVEREST_LIB_DEPS_tls "util;evse_security;framework")
set(EVEREST_LIB_DEPS_helpers "tls;framework")
set(EVEREST_LIB_DEPS_external_energy_limits "framework")
set(EVEREST_LIB_DEPS_everest_api_types "")
set(EVEREST_LIB_DEPS_conversions "framework;evse_security")
set(EVEREST_LIB_DEPS_slac "tls")

# --- Transitive dependency resolver ---
# Given a list of library names, computes the full transitive closure
# of internal dependencies and stores the result in OUTPUT_VAR.
function(_ev_resolve_lib_transitive_deps INPUT_LIBS OUTPUT_VAR)
    set(_resolved "${INPUT_LIBS}")
    set(_frontier "${INPUT_LIBS}")

    while(_frontier)
        set(_next_frontier "")
        foreach(_lib IN LISTS _frontier)
            if(DEFINED EVEREST_LIB_DEPS_${_lib})
                foreach(_dep IN LISTS EVEREST_LIB_DEPS_${_lib})
                    if(NOT _dep IN_LIST _resolved)
                        list(APPEND _resolved "${_dep}")
                        list(APPEND _next_frontier "${_dep}")
                    endif()
                endforeach()
            endif()
        endforeach()
        set(_frontier "${_next_frontier}")
    endwhile()

    set(${OUTPUT_VAR} "${_resolved}" PARENT_SCOPE)
endfunction()

# --- Resolve the library set once at configure time ---
if(EVEREST_INCLUDE_LIBS)
    _ev_resolve_lib_transitive_deps("${EVEREST_INCLUDE_LIBS}" EVEREST_RESOLVED_LIBS)
    message(STATUS "EVEREST_INCLUDE_LIBS set. Requested: ${EVEREST_INCLUDE_LIBS}")
    message(STATUS "  Resolved (with transitive deps): ${EVEREST_RESOLVED_LIBS}")
elseif(EVEREST_EXCLUDE_LIBS)
    message(STATUS "EVEREST_EXCLUDE_LIBS set: ${EVEREST_EXCLUDE_LIBS}")
endif()

# Set per-library build variables (used by cmake_condition in dependencies.yaml)
# When EVEREST_INCLUDE_LIBS is empty these are all ON (build everything).
# Includes the conditional libraries (gpio, system) that have additional guards.
set(_EVEREST_ALL_LIBS ${EVEREST_LIB_SUBDIRS} gpio system)

foreach(_lib IN LISTS _EVEREST_ALL_LIBS)
    if(EVEREST_INCLUDE_LIBS)
        if(_lib IN_LIST EVEREST_RESOLVED_LIBS)
            set(EVEREST_BUILD_LIB_${_lib} ON)
        else()
            set(EVEREST_BUILD_LIB_${_lib} OFF)
        endif()
    elseif(EVEREST_EXCLUDE_LIBS)
        if(_lib IN_LIST EVEREST_EXCLUDE_LIBS)
            set(EVEREST_BUILD_LIB_${_lib} OFF)
        else()
            set(EVEREST_BUILD_LIB_${_lib} ON)
        endif()
    else()
        set(EVEREST_BUILD_LIB_${_lib} ON)
    endif()
endforeach()

# --- Query function ---
# ev_should_build_lib(<lib_name> <output_var>)
# Sets <output_var> to ON or OFF in the caller's scope.
function(ev_should_build_lib LIB_NAME OUTPUT_VAR)
    if(DEFINED EVEREST_BUILD_LIB_${LIB_NAME})
        set(${OUTPUT_VAR} "${EVEREST_BUILD_LIB_${LIB_NAME}}" PARENT_SCOPE)
    else()
        # Unknown library name — default to ON for forwards compatibility
        set(${OUTPUT_VAR} ON PARENT_SCOPE)
    endif()
endfunction()
