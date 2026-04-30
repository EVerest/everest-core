# SO for API with structs and (de)serialization codec
add_library(management_api OBJECT)

target_compile_options(management_api
  PRIVATE  -Wall -Wextra -pedantic -Werror=switch)

target_include_directories(management_api
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(management_api
  PRIVATE
    configuration_api.cpp
    configuration_type_wrapper.cpp
    lifecycle_api.cpp
    lifecycle_type_wrapper.cpp
)

target_link_libraries(management_api
  PUBLIC
    everest::everest_api_types
  PRIVATE
    everest::framework
    everest::log
)

