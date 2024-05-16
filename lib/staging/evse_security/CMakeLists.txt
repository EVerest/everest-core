add_library(evse_security_conversions STATIC)
add_library(everest::evse_security_conversions ALIAS evse_security_conversions)

target_sources(evse_security_conversions
    PRIVATE
        conversions.cpp
)

target_include_directories(evse_security_conversions
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    "$<TARGET_PROPERTY:generate_cpp_files,EVEREST_GENERATED_INCLUDE_DIR>"
)

add_dependencies(evse_security_conversions generate_cpp_files)

target_link_libraries(evse_security_conversions
    PRIVATE
        everest::evse_security
        everest::framework
)
