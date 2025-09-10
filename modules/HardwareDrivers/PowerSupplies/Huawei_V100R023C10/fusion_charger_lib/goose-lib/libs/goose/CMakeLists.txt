file(GLOB_RECURSE GOOSE_SOURCES "src/*.cpp")

find_package(OpenSSL REQUIRED)

add_library(goose STATIC ${GOOSE_SOURCES})
target_include_directories(goose PUBLIC include)
target_link_libraries(goose PUBLIC goose-ethernet OpenSSL::SSL OpenSSL::Crypto Huawei::FusionCharger::LogInterface)

if(EVEREST_CORE_BUILD_TESTING)
  add_subdirectory(tests)
endif()
