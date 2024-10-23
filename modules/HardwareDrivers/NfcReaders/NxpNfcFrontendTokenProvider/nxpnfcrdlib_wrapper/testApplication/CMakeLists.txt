cmake_minimum_required(VERSION 3.15)
project(nxpnfcfrontendDemo
    LANGUAGES CXX)

add_executable(nfc_test_application)
target_sources(nfc_test_application PRIVATE src/nfc_test_application.cpp)
target_compile_definitions(nfc_test_application PRIVATE DEBUG)

target_include_directories(nfc_test_application
    PRIVATE
        include
)

target_link_libraries(nfc_test_application PRIVATE NxpNfcFrontend)
