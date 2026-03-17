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

include(CheckIncludeFile)
check_include_file(net/ethernet.h HAVE_NET_ETHERNET_H)
if(HAVE_NET_ETHERNET_H AND USING_MUSL)
    target_compile_definitions(goose-ethernet PRIVATE _NETINET_IF_ETHER_H)
endif()

if(FUSION_CHARGER_LIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()
