file(GLOB_RECURSE GOOSE_SOURCES "src/*.cpp")

find_package(OpenSSL REQUIRED)

add_library(goose STATIC ${GOOSE_SOURCES})
target_include_directories(goose PUBLIC include)
target_link_libraries(goose PUBLIC goose-ethernet OpenSSL::SSL OpenSSL::Crypto Huawei::FusionCharger::LogInterface)

if(TEST_COVERAGE)
  target_link_libraries(goose-ethernet PRIVATE gcov)
  target_compile_options(goose PRIVATE --coverage)
  target_link_options(goose PRIVATE --coverage)
endif()


if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
