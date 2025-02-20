# OCPP library
add_library(ocpp)
add_library(everest::ocpp ALIAS ocpp)

target_compile_options(ocpp
    PRIVATE
         #-Werror # turn warnings into errors
        -Wimplicit-fallthrough  # avoid unintended fallthroughs
        -pedantic-errors
)

target_compile_definitions(ocpp
    PRIVATE
        MIGRATION_FILE_VERSION_V16=${MIGRATION_FILE_VERSION_V16}
        MIGRATION_FILE_VERSION_V2=${MIGRATION_FILE_VERSION_V2}
        MIGRATION_DEVICE_MODEL_FILE_VERSION_V2=${MIGRATION_DEVICE_MODEL_FILE_VERSION_V2}
)

target_sources(ocpp
    PRIVATE
        ocpp/common/call_types.cpp
        ocpp/common/charging_station_base.cpp
        ocpp/common/ocpp_logging.cpp
        ocpp/common/schemas.cpp
        ocpp/common/types.cpp
        ocpp/common/utils.cpp
        ocpp/common/evse_security_impl.cpp
        ocpp/common/evse_security.cpp
        ocpp/common/database/database_connection.cpp
        ocpp/common/database/database_handler_common.cpp
        ocpp/common/database/database_schema_updater.cpp
        ocpp/common/database/sqlite_statement.cpp
)

if(LIBOCPP_ENABLE_V16)
    target_sources(ocpp
        PRIVATE
            ocpp/v16/charge_point.cpp
            ocpp/v16/database_handler.cpp
            ocpp/v16/charge_point_impl.cpp
            ocpp/v16/message_dispatcher.cpp
            ocpp/v16/smart_charging.cpp
            ocpp/v16/charge_point_configuration.cpp
            ocpp/v16/charge_point_state_machine.cpp
            ocpp/v16/message_queue.cpp
            ocpp/v16/profile.cpp
            ocpp/v16/transaction.cpp
            ocpp/v16/ocpp_enums.cpp
            ocpp/v16/ocpp_types.cpp
            ocpp/v16/types.cpp
            ocpp/v16/utils.cpp
            ocpp/v2/messages/InstallCertificate.cpp
            ocpp/v2/messages/CertificateSigned.cpp
            ocpp/v2/messages/Authorize.cpp
            ocpp/v2/messages/GetCertificateStatus.cpp
            ocpp/v2/messages/Get15118EVCertificate.cpp
            ocpp/v2/messages/DeleteCertificate.cpp
            ocpp/v2/messages/TriggerMessage.cpp
            ocpp/v2/ocpp_enums.cpp
            ocpp/v2/ocpp_types.cpp
            ocpp/v2/types.cpp
            ocpp/v2/messages/SignCertificate.cpp
            ocpp/v2/messages/GetInstalledCertificateIds.cpp
    )
    add_subdirectory(ocpp/v16/messages)
endif()

if(LIBOCPP_ENABLE_V2)
    target_sources(ocpp
        PRIVATE
            ocpp/v2/average_meter_values.cpp
            ocpp/v2/charge_point.cpp
            ocpp/v2/charge_point_callbacks.cpp
            ocpp/v2/connector.cpp
            ocpp/v2/ctrlr_component_variables.cpp
            ocpp/v2/database_handler.cpp
            ocpp/v2/device_model.cpp
            ocpp/v2/device_model_storage_sqlite.cpp
            ocpp/v2/enums.cpp
            ocpp/v2/evse.cpp
            ocpp/v2/evse_manager.cpp
            ocpp/v2/init_device_model_db.cpp
            ocpp/v2/notify_report_requests_splitter.cpp
            ocpp/v2/message_queue.cpp
            ocpp/v2/ocpp_enums.cpp
            ocpp/v2/profile.cpp
            ocpp/v2/ocpp_types.cpp
            ocpp/v2/ocsp_updater.cpp
            ocpp/v2/monitoring_updater.cpp
            ocpp/v2/transaction.cpp
            ocpp/v2/types.cpp
            ocpp/v2/utils.cpp
            ocpp/v2/component_state_manager.cpp
            ocpp/v2/connectivity_manager.cpp
            ocpp/v2/message_dispatcher.cpp
            ocpp/v2/functional_blocks/authorization.cpp
            ocpp/v2/functional_blocks/availability.cpp
            ocpp/v2/functional_blocks/security.cpp
            ocpp/v2/functional_blocks/smart_charging.cpp
            ocpp/v2/functional_blocks/data_transfer.cpp
            ocpp/v2/functional_blocks/firmware_update.cpp
            ocpp/v2/functional_blocks/reservation.cpp
            ocpp/v2/functional_blocks/diagnostics.cpp
            ocpp/v2/functional_blocks/display_message.cpp            
            ocpp/v2/functional_blocks/meter_values.cpp
            ocpp/v2/functional_blocks/provisioning.cpp
            ocpp/v2/functional_blocks/remote_transaction_control.cpp
            ocpp/v2/functional_blocks/tariff_and_cost.cpp
            ocpp/v2/functional_blocks/transaction.cpp
    )
    add_subdirectory(ocpp/v2/messages)
endif()

add_subdirectory(ocpp/common/websocket)

option(LIBOCPP_USE_BOOST_FILESYSTEM "Usage of boost/filesystem.hpp instead of std::filesystem" OFF)

target_include_directories(ocpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_include_directories(ocpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rd_party>
)

#############
# Logging configuration
#
# Set EVEREST_CUSTOM_LOGGING_LIBRARY to use a custom library.
# Alternatively set EVEREST_CUSTOM_LOGGING_INCLUDE_PATH to use a custom header file.
#
# Both options need to make a header file available on path "everest/logging.hpp"
#
# To use the normal library, don't set any of these options.
#
#############
if (EVEREST_CUSTOM_LOGGING_LIBRARY)
    if(NOT TARGET ${EVEREST_CUSTOM_LOGGING_LIBRARY})
        message(FATAL_ERROR "${EVEREST_CUSTOM_LOGGING_LIBRARY} is not a valid library")
    else()
        target_link_libraries(ocpp
            PUBLIC
            ${EVEREST_CUSTOM_LOGGING_LIBRARY}
        )
        message(STATUS "Using custom logging library: ${EVEREST_CUSTOM_LOGGING_LIBRARY}")
    endif()
elseif(EVEREST_CUSTOM_LOGGING_INCLUDE_PATH)
    if (NOT EXISTS "${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
        message(FATAL_ERROR "everest/logging.hpp not found in directory ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}")
    else()
        target_include_directories(ocpp
            PUBLIC
            include
            ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}
        )
        message(STATUS "Using the following logging header: ${EVEREST_CUSTOM_LOGGING_INCLUDE_PATH}/everest/logging.hpp")
    endif()
else()
    target_link_libraries(ocpp
        PUBLIC
        everest::log
    )
    message(STATUS "Using the default logging header")
endif()

#############
# End logging configuration
#############

target_link_libraries(ocpp
    PUBLIC
        everest::timer
        nlohmann_json_schema_validator
        everest::evse_security
        websockets_shared
    PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto
        SQLite::SQLite3
        Threads::Threads

        nlohmann_json::nlohmann_json
        date::date-tz
)

if(LIBOCPP_USE_BOOST_FILESYSTEM)
    find_package(Boost REQUIRED COMPONENTS filesystem)
    target_link_libraries(ocpp
        PRIVATE
            Boost::filesystem
    )
    target_compile_definitions(ocpp
        PRIVATE
            LIBOCPP_USE_BOOST_FILESYSTEM
    )
endif()

# FIXME (aw): right now nlohmann_json and boost::optional don't compile
#             with gcc 10.x and C++11/14, so we need to publish the
#             C++17 standard
target_compile_features(ocpp PUBLIC cxx_std_17)
