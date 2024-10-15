if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

add_subdirectory(libs)
add_subdirectory(examples)

