# detect if we try to build inside everest-core and libcbv2g is available
message(STATUS "Attempting build with autodetected local dependencies")

get_filename_component(EVC_EVEREST_LIB_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(EVC_CBV2G_DIR "${EVC_EVEREST_LIB_DIR}/cbv2g")
if (EVC_CBV2G_DIR AND NOT DISABLE_ISO15118_LOCAL_DEPENDENCIES)
    message(STATUS "Detected libcbv2g in ${EVC_CBV2G_DIR}, if you do not want this set -DDISABLE_ISO15118_LOCAL_DEPENDENCIES=ON")
    add_subdirectory("${EVC_CBV2G_DIR}" libcbv2g)
endif()

# set venv location
set(${PROJECT_NAME}_PYTHON_VENV_PATH "${CMAKE_BINARY_DIR}/venv" CACHE PATH "Path to python venv")

ev_setup_python_executable(
    USE_PYTHON_VENV ${${PROJECT_NAME}_USE_PYTHON_VENV}
    PYTHON_VENV_PATH ${${PROJECT_NAME}_PYTHON_VENV_PATH}
)
get_filename_component(EVC_LIB_DIR ${EVC_EVEREST_LIB_DIR} DIRECTORY)
get_filename_component(EVC_DIR ${EVC_LIB_DIR} DIRECTORY)
set(EVC_EDM_DIR "${EVC_DIR}/applications/dev-environment")

# use edm from everest-core
add_subdirectory("${EVC_EDM_DIR}" edm_tool)
