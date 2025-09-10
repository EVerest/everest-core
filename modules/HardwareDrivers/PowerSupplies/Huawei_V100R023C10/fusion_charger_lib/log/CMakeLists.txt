add_library(huawei-fusion-charger-log-interface-lib logs.cpp)
target_include_directories(huawei-fusion-charger-log-interface-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Huawei::FusionCharger::LogInterface ALIAS huawei-fusion-charger-log-interface-lib)
