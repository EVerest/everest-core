set(EVEREST_CORE_LIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
set(LIB_INCLUDE_TMP_DIR ${EVEREST_CORE_LIB_INCLUDE_DIR}/everest)
set(LIB_INCLUDE_INSTALL_DIR include/everest)

add_subdirectory(3rd_party)
add_subdirectory(everest)
