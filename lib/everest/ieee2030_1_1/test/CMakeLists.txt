add_compile_options(${IEEE2030_COMPILE_OPTIONS_WARNING})
list(APPEND CMAKE_MODULE_PATH ${CPM_PACKAGE_catch2_SOURCE_DIR}/extras)

add_subdirectory(ieee2030)
add_subdirectory(charger)
add_subdirectory(common)
