# Standalone build helper for cbv2g_json_wrapper.
#
# When the wrapper is configured by itself (not as part of an everest-core
# build), the cbv2g::din target does not exist. This script tries to
# locate libcbv2g in three places, in order:
#
#   1. -DCBV2G_SOURCE_DIR=<path> on the cmake command line
#   2. The in-tree path used by everest-core
#      (../../lib/everest/cbv2g relative to this directory)
#   3. find_package(cbv2g) for an installed copy
#
# Modelled on lib/everest/iso15118/cmake/local-build.cmake.
message(STATUS "cbv2g_json_wrapper: standalone configuration detected; "
               "looking for libcbv2g")

if(DEFINED CBV2G_SOURCE_DIR AND EXISTS "${CBV2G_SOURCE_DIR}/CMakeLists.txt")
    message(STATUS "  Using CBV2G_SOURCE_DIR=${CBV2G_SOURCE_DIR}")
    add_subdirectory("${CBV2G_SOURCE_DIR}" libcbv2g_build)
else()
    get_filename_component(_wrapper_root "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)
    set(_in_tree_cbv2g "${_wrapper_root}/../../lib/everest/cbv2g")
    get_filename_component(_in_tree_cbv2g "${_in_tree_cbv2g}" ABSOLUTE)

    if(EXISTS "${_in_tree_cbv2g}/CMakeLists.txt")
        message(STATUS "  Using in-tree libcbv2g at ${_in_tree_cbv2g}")
        add_subdirectory("${_in_tree_cbv2g}" libcbv2g_build)
    else()
        find_package(cbv2g QUIET)
        if(NOT TARGET cbv2g::din)
            message(FATAL_ERROR
                "cbv2g::din target not found. Pass -DCBV2G_SOURCE_DIR=<path "
                "to libcbv2g checkout> or install libcbv2g via "
                "find_package(cbv2g).")
        endif()
    endif()
endif()
