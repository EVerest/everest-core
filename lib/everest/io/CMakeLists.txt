option(BUILD_DOC "Build documentation" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(test)
endif()

if (BUILD_DOC)
  add_subdirectory(docs)
endif()

if (BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
