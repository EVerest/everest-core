if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
endif()

add_subdirectory(libs)
add_subdirectory(examples)
