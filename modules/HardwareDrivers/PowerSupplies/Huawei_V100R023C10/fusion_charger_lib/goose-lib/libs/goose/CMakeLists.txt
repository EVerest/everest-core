file(GLOB_RECURSE GOOSE_SOURCES "src/*.cpp")

find_package(OpenSSL REQUIRED)

add_library(goose STATIC ${GOOSE_SOURCES})
target_include_directories(goose PUBLIC include)
target_link_libraries(goose PUBLIC goose-ethernet OpenSSL::SSL OpenSSL::Crypto Huawei::FusionCharger::LogInterface)

if(FUSION_CHARGER_LIB_BUILD_TESTS)
  add_subdirectory(tests)
endif()
