file(GLOB_RECURSE GOOSE_ETHERNET_SOURCES "src/*.cpp")

if (MACOSX)
  list(FILTER GOOSE_ETHERNET_SOURCES EXCLUDE REGEX ".+linux\.cpp")
else()
  list(FILTER GOOSE_ETHERNET_SOURCES EXCLUDE REGEX ".+mac\.cpp")
endif()

add_library(goose-ethernet STATIC ${GOOSE_ETHERNET_SOURCES})
target_include_directories(goose-ethernet PUBLIC include)

if (MACOSX)
  target_link_libraries(goose-ethernet PRIVATE pcap)
endif()

if(FUSION_CHARGER_LIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()
