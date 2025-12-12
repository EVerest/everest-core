find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE MODBUS_SSL_SOURCES "src/*.cpp")

add_library(modbus-ssl STATIC ${MODBUS_SSL_SOURCES})

target_link_libraries(
    modbus-ssl PUBLIC modbus-base OpenSSL::SSL OpenSSL::Crypto
)
target_include_directories(modbus-ssl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
