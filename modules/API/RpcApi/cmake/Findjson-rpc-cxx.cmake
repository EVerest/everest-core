#[=======================================================================[.rst:
Findjson-rpc-cxx
----------------

Finds the json-rpc-cxx library.

json-rpc-cxx is a header-only JSON-RPC 2.0 framework implemented in C++17
using nlohmann's json for modern C++.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``json-rpc-cxx::json-rpc-cxx``
  The json-rpc-cxx library
``json-rpc-cxx``
  Alias for json-rpc-cxx::json-rpc-cxx

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``json-rpc-cxx_FOUND``
  True if the system has the json-rpc-cxx library.
``json-rpc-cxx_INCLUDE_DIRS``
  Include directories needed to use json-rpc-cxx.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``json-rpc-cxx_INCLUDE_DIR``
  The directory containing ``jsonrpccxx/common.hpp``.

#]=======================================================================]

# Find nlohmann_json dependency first
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    # Try to find nlohmann/json with alternative names
    find_package(nlohmann-json QUIET)
endif()

# Look for the header file
find_path(json-rpc-cxx_INCLUDE_DIR
    NAMES jsonrpccxx/common.hpp
    PATHS
        ${CMAKE_PREFIX_PATH}/include
        /usr/local/include
        /usr/include
        ${json-rpc-cxx_ROOT}/include
        $ENV{json-rpc-cxx_ROOT}/include
    PATH_SUFFIXES
        jsonrpccxx
    DOC "json-rpc-cxx include directory"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(json-rpc-cxx
    FOUND_VAR json-rpc-cxx_FOUND
    REQUIRED_VARS
        json-rpc-cxx_INCLUDE_DIR
)

if(json-rpc-cxx_FOUND)
    set(json-rpc-cxx_INCLUDE_DIRS ${json-rpc-cxx_INCLUDE_DIR})

    # Create imported target
    if(NOT TARGET json-rpc-cxx::json-rpc-cxx)
        add_library(json-rpc-cxx::json-rpc-cxx INTERFACE IMPORTED)
        set_target_properties(json-rpc-cxx::json-rpc-cxx PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${json-rpc-cxx_INCLUDE_DIR}"
        )

        # Add C++17 requirement
        set_target_properties(json-rpc-cxx::json-rpc-cxx PROPERTIES
            INTERFACE_COMPILE_FEATURES cxx_std_17
        )

        # Link nlohmann_json dependency if found
        if(TARGET nlohmann_json::nlohmann_json)
            set_target_properties(json-rpc-cxx::json-rpc-cxx PROPERTIES
                INTERFACE_LINK_LIBRARIES nlohmann_json::nlohmann_json
            )
        elseif(TARGET nlohmann_json)
            set_target_properties(json-rpc-cxx::json-rpc-cxx PROPERTIES
                INTERFACE_LINK_LIBRARIES nlohmann_json
            )
        endif()

        # Create alias for simpler target name
        add_library(json-rpc-cxx ALIAS json-rpc-cxx::json-rpc-cxx)
    endif()
endif()

mark_as_advanced(
    json-rpc-cxx_INCLUDE_DIR
)
