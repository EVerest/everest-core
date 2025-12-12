file(GLOB SOURCES "src/*.cpp")

add_library(modbus-registers STATIC ${SOURCES})
target_include_directories(modbus-registers PUBLIC include)
target_link_libraries(modbus-registers PUBLIC modbus-server)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
