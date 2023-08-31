
add_library(evse_security)
add_library(everest::evse_security ALIAS evse_security)

target_sources(evse_security
    PRIVATE
        evse_security.cpp
        x509_wrapper.cpp
)

target_include_directories(evse_security
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(evse_security
    PUBLIC
        everest::log
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
)

target_compile_features(evse_security PUBLIC cxx_std_17)
