if(NOT TARGET Boost::system)
    add_library(everest_boost_system_target INTERFACE)
    add_library(Boost::system ALIAS everest_boost_system_target)
endif()
