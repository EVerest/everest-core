add_executable(main main.cpp)

target_link_libraries(main
    PRIVATE
        everest::log
        Boost::program_options
)

# needs c++ 14
target_compile_features(main PRIVATE cxx_std_14)

install(TARGETS main
        RUNTIME)

install(FILES logging.ini
        DESTINATION ".")
