# OCPP EVSE Security

add_library(ocpp_evse_security STATIC)
add_library(everest::ocpp_evse_security ALIAS ocpp_evse_security)

target_sources(ocpp_evse_security
    PRIVATE
        evse_security_ocpp.cpp
)

target_include_directories(ocpp_evse_security
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

target_compile_options(ocpp_evse_security
    PRIVATE
        -Wimplicit-fallthrough
        -Werror=switch-enum
)

add_dependencies(ocpp_evse_security generate_cpp_files)

target_link_libraries(ocpp_evse_security
    PRIVATE
        everest::ocpp
        everest::framework
)


# OCPP conversions

add_library(ocpp_conversions STATIC)
add_library(everest::ocpp_conversions ALIAS ocpp_conversions)

target_sources(ocpp_conversions
    PRIVATE
        ocpp_conversions.cpp
)

target_include_directories(ocpp_conversions
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

target_compile_options(ocpp_conversions
    PRIVATE
        -Wimplicit-fallthrough
        -Werror=switch-enum
)

add_dependencies(ocpp_conversions generate_cpp_files)

target_link_libraries(ocpp_conversions
    PRIVATE
        everest::ocpp
        everest::framework
)

