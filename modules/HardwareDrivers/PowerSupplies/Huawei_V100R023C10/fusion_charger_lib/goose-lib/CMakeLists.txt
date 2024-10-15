if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
endif()

add_subdirectory(libs)
add_subdirectory(examples)
