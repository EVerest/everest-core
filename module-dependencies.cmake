#
# define which modules need a certain dependency
#

ev_define_dependency(
    DEPENDENCY_NAME sigslot
    DEPENDENT_MODULES_LIST EnergyNode EvseManager MicroMegaWattBSP YetiDriver)

ev_define_dependency(
    DEPENDENCY_NAME pugixml
    DEPENDENT_MODULES_LIST EvseManager)

ev_define_dependency(
    DEPENDENCY_NAME libtimer
    DEPENDENT_MODULES_LIST Auth LemDCBM400600 System)

ev_define_dependency(
    DEPENDENCY_NAME libcurl
    DEPENDENT_MODULES_LIST LemDCBM400600 IsabellenhuetteIemDcr)

ev_define_dependency(
    DEPENDENCY_NAME libocpp
    DEPENDENT_MODULES_LIST OCPP OCPP201)

ev_define_dependency(
    DEPENDENCY_NAME Josev
    DEPENDENT_MODULES_LIST PyEvJosev)

ev_define_dependency(
    DEPENDENCY_NAME libcbv2g
    OUTPUT_VARIABLE_SUFFIX LIBCBV2G
    DEPENDENT_MODULES_LIST EvseV2G)

ev_define_dependency(
    DEPENDENCY_NAME libevse-security
    OUTPUT_VARIABLE_SUFFIX LIBEVSE_SECURITY
    DEPENDENT_MODULES_LIST OCPP OCPP201 EvseSecurity EvseV2G)

ev_define_dependency(
    DEPENDENCY_NAME everest-sqlite
    DEPENDENT_MODULES_LIST ErrorHistory)

ev_define_dependency(
    DEPENDENCY_NAME libiso15118
    OUTPUT_VARIABLE_SUFFIX LIBISO15118
    DEPENDENT_MODULES_LIST Evse15118D20)

ev_define_dependency(
    DEPENDENCY_NAME libnfc-nci
    OUTPUT_VARIABLE_SUFFIX LIBNFC_NCI
    DEPENDENT_MODULES_LIST PN7160TokenProvider)

ev_define_dependency(
    DEPENDENCY_NAME ftxui
    DEPENDENT_MODULES_LIST BUDisplayMessage BUEvseBoardSupport BUIsolationMonitor BUmMWcar BUNfcReader BUOverVoltageMonitor BUPowermeter BUPowerSupplyDC BUSystem)

ev_define_dependency(
    DEPENDENCY_NAME sdbus-cpp
    OUTPUT_VARIABLE_SUFFIX SDBUS_CPP
    DEPENDENT_MODULES_LIST Linux_Systemd_Rauc)

ev_define_dependency(
    DEPENDENCY_NAME json-rpc-cxx
    OUTPUT_VARIABLE_SUFFIX JSON_RPC_CXX
    DEPENDENT_MODULES_LIST RpcApi)

if(NOT everest-gpio IN_LIST EVEREST_EXCLUDE_DEPENDENCIES)
    set(EVEREST_DEPENDENCY_ENABLED_EVEREST_GPIO ON)
else()
    set(EVEREST_DEPENDENCY_ENABLED_EVEREST_GPIO OFF)
    message(STATUS "Dependency everest-gpio NOT enabled")
endif()

