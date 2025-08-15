// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <conversions.hpp>
#include <everest/conversions/ocpp/ocpp_conversions.hpp>
#include <everest/logging.hpp>

namespace module {
namespace conversions {
ocpp::v2::FirmwareStatusEnum to_ocpp_firmware_status_enum(const types::system::FirmwareUpdateStatusEnum status) {
    switch (status) {
    case types::system::FirmwareUpdateStatusEnum::Downloaded:
        return ocpp::v2::FirmwareStatusEnum::Downloaded;
    case types::system::FirmwareUpdateStatusEnum::DownloadFailed:
        return ocpp::v2::FirmwareStatusEnum::DownloadFailed;
    case types::system::FirmwareUpdateStatusEnum::Downloading:
        return ocpp::v2::FirmwareStatusEnum::Downloading;
    case types::system::FirmwareUpdateStatusEnum::DownloadScheduled:
        return ocpp::v2::FirmwareStatusEnum::DownloadScheduled;
    case types::system::FirmwareUpdateStatusEnum::DownloadPaused:
        return ocpp::v2::FirmwareStatusEnum::DownloadPaused;
    case types::system::FirmwareUpdateStatusEnum::Idle:
        return ocpp::v2::FirmwareStatusEnum::Idle;
    case types::system::FirmwareUpdateStatusEnum::InstallationFailed:
        return ocpp::v2::FirmwareStatusEnum::InstallationFailed;
    case types::system::FirmwareUpdateStatusEnum::Installing:
        return ocpp::v2::FirmwareStatusEnum::Installing;
    case types::system::FirmwareUpdateStatusEnum::Installed:
        return ocpp::v2::FirmwareStatusEnum::Installed;
    case types::system::FirmwareUpdateStatusEnum::InstallRebooting:
        return ocpp::v2::FirmwareStatusEnum::InstallRebooting;
    case types::system::FirmwareUpdateStatusEnum::InstallScheduled:
        return ocpp::v2::FirmwareStatusEnum::InstallScheduled;
    case types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed:
        return ocpp::v2::FirmwareStatusEnum::InstallVerificationFailed;
    case types::system::FirmwareUpdateStatusEnum::InvalidSignature:
        return ocpp::v2::FirmwareStatusEnum::InvalidSignature;
    case types::system::FirmwareUpdateStatusEnum::SignatureVerified:
        return ocpp::v2::FirmwareStatusEnum::SignatureVerified;
    }
    throw std::out_of_range("Could not convert FirmwareUpdateStatusEnum to FirmwareStatusEnum");
}

ocpp::v2::DataTransferStatusEnum to_ocpp_data_transfer_status_enum(types::ocpp::DataTransferStatus status) {
    switch (status) {
    case types::ocpp::DataTransferStatus::Accepted:
        return ocpp::v2::DataTransferStatusEnum::Accepted;
    case types::ocpp::DataTransferStatus::Rejected:
        return ocpp::v2::DataTransferStatusEnum::Rejected;
    case types::ocpp::DataTransferStatus::UnknownMessageId:
        return ocpp::v2::DataTransferStatusEnum::UnknownMessageId;
    case types::ocpp::DataTransferStatus::UnknownVendorId:
        return ocpp::v2::DataTransferStatusEnum::UnknownVendorId;
    case types::ocpp::DataTransferStatus::Offline:
        return ocpp::v2::DataTransferStatusEnum::UnknownVendorId;
    }
    return ocpp::v2::DataTransferStatusEnum::UnknownVendorId;
}

ocpp::v2::DataTransferRequest to_ocpp_data_transfer_request(types::ocpp::DataTransferRequest request) {
    ocpp::v2::DataTransferRequest ocpp_request;
    ocpp_request.vendorId = request.vendor_id;
    if (request.message_id.has_value()) {
        ocpp_request.messageId = request.message_id.value();
    }
    if (request.data.has_value()) {
        try {
            ocpp_request.data = json::parse(request.data.value());
        } catch (const json::exception& e) {
            EVLOG_error << "Parsing of data transfer request data json failed because: "
                        << "(" << e.what() << ")";
        }
    }
    if (request.custom_data.has_value()) {
        auto custom_data = request.custom_data.value();
        try {
            json custom_data_json = json::parse(custom_data.data);
            if (not custom_data_json.contains("vendorId")) {
                EVLOG_warning
                    << "DataTransferRequest custom_data.data does not contain vendorId, automatically adding it";
                custom_data_json["vendorId"] = custom_data.vendor_id;
            }
            ocpp_request.customData = custom_data_json;
        } catch (const json::exception& e) {
            EVLOG_error << "Parsing of data transfer request custom_data json failed because: "
                        << "(" << e.what() << ")";
        }
    }
    return ocpp_request;
}

ocpp::v2::DataTransferResponse to_ocpp_data_transfer_response(types::ocpp::DataTransferResponse response) {
    ocpp::v2::DataTransferResponse ocpp_response;
    ocpp_response.status = conversions::to_ocpp_data_transfer_status_enum(response.status);
    if (response.data.has_value()) {
        ocpp_response.data = json::parse(response.data.value());
    }
    if (response.custom_data.has_value()) {
        auto custom_data = response.custom_data.value();
        json custom_data_json = json::parse(custom_data.data);
        if (not custom_data_json.contains("vendorId")) {
            EVLOG_warning << "DataTransferResponse custom_data.data does not contain vendorId, automatically adding it";
            custom_data_json["vendorId"] = custom_data.vendor_id;
        }
        ocpp_response.customData = custom_data_json;
    }
    return ocpp_response;
}

ocpp::v2::SampledValue to_ocpp_sampled_value(const ocpp::v2::ReadingContextEnum& reading_context,
                                             const ocpp::v2::MeasurandEnum& measurand, const std::string& unit,
                                             const std::optional<ocpp::v2::PhaseEnum> phase,
                                             ocpp::v2::LocationEnum location) {
    ocpp::v2::SampledValue sampled_value;
    ocpp::v2::UnitOfMeasure unit_of_measure;
    sampled_value.context = reading_context;
    sampled_value.location = location;
    sampled_value.measurand = measurand;
    unit_of_measure.unit = unit;
    sampled_value.unitOfMeasure = unit_of_measure;
    sampled_value.phase = phase;
    return sampled_value;
}

ocpp::v2::SignedMeterValue to_ocpp_signed_meter_value(const types::units_signed::SignedMeterValue& signed_meter_value) {
    ocpp::v2::SignedMeterValue ocpp_signed_meter_value;
    ocpp_signed_meter_value.signedMeterData = signed_meter_value.signed_meter_data;
    ocpp_signed_meter_value.signingMethod = signed_meter_value.signing_method;
    ocpp_signed_meter_value.encodingMethod = signed_meter_value.encoding_method;
    ocpp_signed_meter_value.publicKey = signed_meter_value.public_key.value_or("");

    return ocpp_signed_meter_value;
}

ocpp::v2::MeterValue
to_ocpp_meter_value(const types::powermeter::Powermeter& power_meter,
                    const ocpp::v2::ReadingContextEnum& reading_context,
                    const std::optional<types::units_signed::SignedMeterValue> signed_meter_value) {
    ocpp::v2::MeterValue meter_value;
    meter_value.timestamp = ocpp_conversions::to_ocpp_datetime_or_now(power_meter.timestamp);

    bool energy_Wh_import_signed_total_added = false;
    // individual signed meter values can be provided by the power_meter itself

    ocpp::v2::SampledValue sampled_value = to_ocpp_sampled_value(
        reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Import_Register, "Wh", std::nullopt);

    // Energy.Active.Import.Register
    if (power_meter.energy_Wh_import_signed.has_value()) {
        sampled_value.value = power_meter.energy_Wh_import.total;
        const auto& energy_Wh_import_signed = power_meter.energy_Wh_import_signed.value();
        if (energy_Wh_import_signed.total.has_value()) {
            sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_import_signed.total.value());
            energy_Wh_import_signed_total_added = true;
        }
        meter_value.sampledValue.push_back(sampled_value);
    }

    if (not energy_Wh_import_signed_total_added) {
        // No signed meter value for Energy.Active.Import.Register added, either no signed meter values are available or
        // just one global signed_meter_value is present signed_meter_value is intended for OCMF style blobs of signed
        // meter value reports during transaction start or end
        // This is interpreted as Energy.Active.Import.Register
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", std::nullopt);
        sampled_value.value = power_meter.energy_Wh_import.total;
        // add signedMeterValue if present
        if (signed_meter_value.has_value()) {
            sampled_value.signedMeterValue = to_ocpp_signed_meter_value(signed_meter_value.value());
        }
        meter_value.sampledValue.push_back(sampled_value);
    }

    if (power_meter.energy_Wh_import.L1.has_value()) {
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v2::PhaseEnum::L1);
        sampled_value.value = power_meter.energy_Wh_import.L1.value();
        if (power_meter.energy_Wh_import_signed.has_value()) {
            const auto& energy_Wh_import_signed = power_meter.energy_Wh_import_signed.value();
            if (energy_Wh_import_signed.L1.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_import_signed.L1.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L2.has_value()) {
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v2::PhaseEnum::L2);
        sampled_value.value = power_meter.energy_Wh_import.L2.value();
        if (power_meter.energy_Wh_import_signed.has_value()) {
            const auto& energy_Wh_import_signed = power_meter.energy_Wh_import_signed.value();
            if (energy_Wh_import_signed.L2.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_import_signed.L2.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
    }
    if (power_meter.energy_Wh_import.L3.has_value()) {
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v2::PhaseEnum::L3);
        sampled_value.value = power_meter.energy_Wh_import.L3.value();
        if (power_meter.energy_Wh_import_signed.has_value()) {
            const auto& energy_Wh_import_signed = power_meter.energy_Wh_import_signed.value();
            if (energy_Wh_import_signed.L3.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_import_signed.L3.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
    }

    // Energy.Active.Export.Register
    if (power_meter.energy_Wh_export.has_value()) {
        auto sampled_value = to_ocpp_sampled_value(
            reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Export_Register, "Wh", std::nullopt);
        sampled_value.value = power_meter.energy_Wh_export.value().total;
        if (power_meter.energy_Wh_export_signed.has_value()) {
            const auto& energy_Wh_export_signed = power_meter.energy_Wh_export_signed.value();
            if (energy_Wh_export_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_export_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.energy_Wh_export.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(
                reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Export_Register, "Wh", ocpp::v2::PhaseEnum::L1);
            sampled_value.value = power_meter.energy_Wh_export.value().L1.value();
            if (power_meter.energy_Wh_export_signed.has_value()) {
                const auto& energy_Wh_export_signed = power_meter.energy_Wh_export_signed.value();
                if (energy_Wh_export_signed.L1.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_export_signed.L1.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L2.has_value()) {
            sampled_value = to_ocpp_sampled_value(
                reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Export_Register, "Wh", ocpp::v2::PhaseEnum::L2);
            sampled_value.value = power_meter.energy_Wh_export.value().L2.value();
            if (power_meter.energy_Wh_export_signed.has_value()) {
                const auto& energy_Wh_export_signed = power_meter.energy_Wh_export_signed.value();
                if (energy_Wh_export_signed.L2.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_export_signed.L2.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.energy_Wh_export.value().L3.has_value()) {
            sampled_value = to_ocpp_sampled_value(
                reading_context, ocpp::v2::MeasurandEnum::Energy_Active_Export_Register, "Wh", ocpp::v2::PhaseEnum::L3);
            sampled_value.value = power_meter.energy_Wh_export.value().L3.value();
            if (power_meter.energy_Wh_export_signed.has_value()) {
                const auto& energy_Wh_export_signed = power_meter.energy_Wh_export_signed.value();
                if (energy_Wh_export_signed.L3.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_export_signed.L3.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Active.Import
    if (power_meter.power_W.has_value()) {
        auto sampled_value =
            to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Active_Import, "W", std::nullopt);
        sampled_value.value = power_meter.power_W.value().total;
        if (power_meter.power_W_signed.has_value()) {
            const auto& power_W_signed = power_meter.power_W_signed.value();
            if (power_W_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(power_W_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.power_W.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v2::PhaseEnum::L1);
            sampled_value.value = power_meter.power_W.value().L1.value();
            if (power_meter.power_W_signed.has_value()) {
                const auto& power_W_signed = power_meter.power_W_signed.value();
                if (power_W_signed.L1.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(power_W_signed.L1.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L2.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v2::PhaseEnum::L2);
            sampled_value.value = power_meter.power_W.value().L2.value();
            if (power_meter.power_W_signed.has_value()) {
                const auto& power_W_signed = power_meter.power_W_signed.value();
                if (power_W_signed.L2.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(power_W_signed.L2.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.power_W.value().L3.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v2::PhaseEnum::L3);
            sampled_value.value = power_meter.power_W.value().L3.value();
            if (power_meter.power_W_signed.has_value()) {
                const auto& power_W_signed = power_meter.power_W_signed.value();
                if (power_W_signed.L3.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(power_W_signed.L3.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Power.Reactive.Import
    if (power_meter.VAR.has_value()) {
        auto sampled_value =
            to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Reactive_Import, "var", std::nullopt);
        sampled_value.value = power_meter.VAR.value().total;
        if (power_meter.VAR_signed.has_value()) {
            const auto& VAR_signed = power_meter.VAR_signed.value();
            if (VAR_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(VAR_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.VAR.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v2::PhaseEnum::L1);
            sampled_value.value = power_meter.VAR.value().L1.value();
            if (power_meter.VAR_signed.has_value()) {
                const auto& VAR_signed = power_meter.VAR_signed.value();
                if (VAR_signed.L1.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(VAR_signed.L1.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L2.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v2::PhaseEnum::L2);
            sampled_value.value = power_meter.VAR.value().L2.value();
            if (power_meter.VAR_signed.has_value()) {
                const auto& VAR_signed = power_meter.VAR_signed.value();
                if (VAR_signed.L2.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(VAR_signed.L2.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.VAR.value().L3.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v2::PhaseEnum::L3);
            sampled_value.value = power_meter.VAR.value().L3.value();
            if (power_meter.VAR_signed.has_value()) {
                const auto& VAR_signed = power_meter.VAR_signed.value();
                if (VAR_signed.L3.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(VAR_signed.L3.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Current.Import
    if (power_meter.current_A.has_value()) {
        auto sampled_value =
            to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A", std::nullopt);
        if (power_meter.current_A.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v2::PhaseEnum::L1);
            sampled_value.value = power_meter.current_A.value().L1.value();
            if (power_meter.current_A_signed.has_value()) {
                const auto& current_A_signed = power_meter.current_A_signed.value();
                if (current_A_signed.L1.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(current_A_signed.L1.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L2.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v2::PhaseEnum::L2);
            sampled_value.value = power_meter.current_A.value().L2.value();
            if (power_meter.current_A_signed.has_value()) {
                const auto& current_A_signed = power_meter.current_A_signed.value();
                if (current_A_signed.L2.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(current_A_signed.L2.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().L3.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v2::PhaseEnum::L3);
            sampled_value.value = power_meter.current_A.value().L3.value();
            if (power_meter.current_A_signed.has_value()) {
                const auto& current_A_signed = power_meter.current_A_signed.value();
                if (current_A_signed.L3.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(current_A_signed.L3.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().DC.has_value()) {
            sampled_value =
                to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A", std::nullopt);
            sampled_value.value = power_meter.current_A.value().DC.value();
            if (power_meter.current_A_signed.has_value()) {
                const auto& current_A_signed = power_meter.current_A_signed.value();
                if (current_A_signed.DC.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(current_A_signed.DC.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.current_A.value().N.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v2::PhaseEnum::N);
            sampled_value.value = power_meter.current_A.value().N.value();
            if (power_meter.current_A_signed.has_value()) {
                const auto& current_A_signed = power_meter.current_A_signed.value();
                if (current_A_signed.N.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(current_A_signed.N.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
    }

    // Voltage
    if (power_meter.voltage_V.has_value()) {
        if (power_meter.voltage_V.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Voltage, "V",
                                                  ocpp::v2::PhaseEnum::L1_N);
            sampled_value.value = power_meter.voltage_V.value().L1.value();
            if (power_meter.voltage_V_signed.has_value()) {
                const auto& voltage_V_signed = power_meter.voltage_V_signed.value();
                if (voltage_V_signed.L1.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(voltage_V_signed.L1.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L2.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Voltage, "V",
                                                  ocpp::v2::PhaseEnum::L2_N);
            sampled_value.value = power_meter.voltage_V.value().L2.value();
            if (power_meter.voltage_V_signed.has_value()) {
                const auto& voltage_V_signed = power_meter.voltage_V_signed.value();
                if (voltage_V_signed.L2.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(voltage_V_signed.L2.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().L3.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Voltage, "V",
                                                  ocpp::v2::PhaseEnum::L3_N);
            sampled_value.value = power_meter.voltage_V.value().L3.value();
            if (power_meter.voltage_V_signed.has_value()) {
                const auto& voltage_V_signed = power_meter.voltage_V_signed.value();
                if (voltage_V_signed.L3.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(voltage_V_signed.L3.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
        if (power_meter.voltage_V.value().DC.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v2::MeasurandEnum::Voltage, "V", std::nullopt);
            sampled_value.value = power_meter.voltage_V.value().DC.value();
            if (power_meter.voltage_V_signed.has_value()) {
                const auto& voltage_V_signed = power_meter.voltage_V_signed.value();
                if (voltage_V_signed.DC.has_value()) {
                    sampled_value.signedMeterValue = to_ocpp_signed_meter_value(voltage_V_signed.DC.value());
                }
            }
            meter_value.sampledValue.push_back(sampled_value);
        }
    }
    return meter_value;
}

ocpp::v2::LogStatusEnum to_ocpp_log_status_enum(types::system::UploadLogsStatus log_status) {
    switch (log_status) {
    case types::system::UploadLogsStatus::Accepted:
        return ocpp::v2::LogStatusEnum::Accepted;
    case types::system::UploadLogsStatus::Rejected:
        return ocpp::v2::LogStatusEnum::Rejected;
    case types::system::UploadLogsStatus::AcceptedCanceled:
        return ocpp::v2::LogStatusEnum::AcceptedCanceled;
    }
    throw std::runtime_error("Could not convert UploadLogsStatus");
}

ocpp::v2::GetLogResponse to_ocpp_get_log_response(const types::system::UploadLogsResponse& response) {
    ocpp::v2::GetLogResponse _response;
    _response.status = to_ocpp_log_status_enum(response.upload_logs_status);

    if (response.file_name.has_value()) {
        // we just truncate here since the upload operation could have already been started by the system module and
        // we cant do much about it, so best we can do is truncate the filename and rather make sure in the system
        // module that shorter filenames are used
        _response.filename = ocpp::CiString<255>(response.file_name.value(), ocpp::StringTooLarge::Truncate);
    }
    return _response;
}

ocpp::v2::UpdateFirmwareStatusEnum
to_ocpp_update_firmware_status_enum(const types::system::UpdateFirmwareResponse& response) {
    switch (response) {
    case types::system::UpdateFirmwareResponse::Accepted:
        return ocpp::v2::UpdateFirmwareStatusEnum::Accepted;
    case types::system::UpdateFirmwareResponse::Rejected:
        return ocpp::v2::UpdateFirmwareStatusEnum::Rejected;
    case types::system::UpdateFirmwareResponse::AcceptedCanceled:
        return ocpp::v2::UpdateFirmwareStatusEnum::AcceptedCanceled;
    case types::system::UpdateFirmwareResponse::InvalidCertificate:
        return ocpp::v2::UpdateFirmwareStatusEnum::InvalidCertificate;
    case types::system::UpdateFirmwareResponse::RevokedCertificate:
        return ocpp::v2::UpdateFirmwareStatusEnum::RevokedCertificate;
    }
    throw std::runtime_error("Could not convert UpdateFirmwareResponse");
}

ocpp::v2::UpdateFirmwareResponse
to_ocpp_update_firmware_response(const types::system::UpdateFirmwareResponse& response) {
    ocpp::v2::UpdateFirmwareResponse _response;
    _response.status = conversions::to_ocpp_update_firmware_status_enum(response);
    return _response;
}

ocpp::v2::UploadLogStatusEnum to_ocpp_upload_logs_status_enum(types::system::LogStatusEnum status) {
    switch (status) {
    case types::system::LogStatusEnum::BadMessage:
        return ocpp::v2::UploadLogStatusEnum::BadMessage;
    case types::system::LogStatusEnum::Idle:
        return ocpp::v2::UploadLogStatusEnum::Idle;
    case types::system::LogStatusEnum::NotSupportedOperation:
        return ocpp::v2::UploadLogStatusEnum::NotSupportedOperation;
    case types::system::LogStatusEnum::PermissionDenied:
        return ocpp::v2::UploadLogStatusEnum::PermissionDenied;
    case types::system::LogStatusEnum::Uploaded:
        return ocpp::v2::UploadLogStatusEnum::Uploaded;
    case types::system::LogStatusEnum::UploadFailure:
        return ocpp::v2::UploadLogStatusEnum::UploadFailure;
    case types::system::LogStatusEnum::Uploading:
        return ocpp::v2::UploadLogStatusEnum::Uploading;
    case types::system::LogStatusEnum::AcceptedCanceled:
        return ocpp::v2::UploadLogStatusEnum::AcceptedCanceled;
    }
    throw std::runtime_error("Could not convert UploadLogStatusEnum");
}

ocpp::v2::BootReasonEnum to_ocpp_boot_reason(types::system::BootReason reason) {
    switch (reason) {
    case types::system::BootReason::ApplicationReset:
        return ocpp::v2::BootReasonEnum::ApplicationReset;
    case types::system::BootReason::FirmwareUpdate:
        return ocpp::v2::BootReasonEnum::FirmwareUpdate;
    case types::system::BootReason::LocalReset:
        return ocpp::v2::BootReasonEnum::LocalReset;
    case types::system::BootReason::PowerUp:
        return ocpp::v2::BootReasonEnum::PowerUp;
    case types::system::BootReason::RemoteReset:
        return ocpp::v2::BootReasonEnum::RemoteReset;
    case types::system::BootReason::ScheduledReset:
        return ocpp::v2::BootReasonEnum::ScheduledReset;
    case types::system::BootReason::Triggered:
        return ocpp::v2::BootReasonEnum::Triggered;
    case types::system::BootReason::Unknown:
        return ocpp::v2::BootReasonEnum::Unknown;
    case types::system::BootReason::Watchdog:
        return ocpp::v2::BootReasonEnum::Watchdog;
    }
    throw std::runtime_error("Could not convert BootReasonEnum");
}

ocpp::v2::ReasonEnum to_ocpp_reason(types::evse_manager::StopTransactionReason reason) {
    switch (reason) {
    case types::evse_manager::StopTransactionReason::DeAuthorized:
        return ocpp::v2::ReasonEnum::DeAuthorized;
    case types::evse_manager::StopTransactionReason::EmergencyStop:
        return ocpp::v2::ReasonEnum::EmergencyStop;
    case types::evse_manager::StopTransactionReason::EnergyLimitReached:
        return ocpp::v2::ReasonEnum::EnergyLimitReached;
    case types::evse_manager::StopTransactionReason::EVDisconnected:
        return ocpp::v2::ReasonEnum::EVDisconnected;
    case types::evse_manager::StopTransactionReason::GroundFault:
        return ocpp::v2::ReasonEnum::GroundFault;
    case types::evse_manager::StopTransactionReason::HardReset:
        return ocpp::v2::ReasonEnum::ImmediateReset;
    case types::evse_manager::StopTransactionReason::Local:
        return ocpp::v2::ReasonEnum::Local;
    case types::evse_manager::StopTransactionReason::LocalOutOfCredit:
        return ocpp::v2::ReasonEnum::LocalOutOfCredit;
    case types::evse_manager::StopTransactionReason::MasterPass:
        return ocpp::v2::ReasonEnum::MasterPass;
    case types::evse_manager::StopTransactionReason::Other:
        return ocpp::v2::ReasonEnum::Other;
    case types::evse_manager::StopTransactionReason::OvercurrentFault:
        return ocpp::v2::ReasonEnum::OvercurrentFault;
    case types::evse_manager::StopTransactionReason::PowerLoss:
        return ocpp::v2::ReasonEnum::PowerLoss;
    case types::evse_manager::StopTransactionReason::PowerQuality:
        return ocpp::v2::ReasonEnum::PowerQuality;
    case types::evse_manager::StopTransactionReason::Reboot:
        return ocpp::v2::ReasonEnum::Reboot;
    case types::evse_manager::StopTransactionReason::Remote:
        return ocpp::v2::ReasonEnum::Remote;
    case types::evse_manager::StopTransactionReason::SOCLimitReached:
        return ocpp::v2::ReasonEnum::SOCLimitReached;
    case types::evse_manager::StopTransactionReason::StoppedByEV:
        return ocpp::v2::ReasonEnum::StoppedByEV;
    case types::evse_manager::StopTransactionReason::TimeLimitReached:
        return ocpp::v2::ReasonEnum::TimeLimitReached;
    case types::evse_manager::StopTransactionReason::Timeout:
        return ocpp::v2::ReasonEnum::Timeout;
    case types::evse_manager::StopTransactionReason::ReqEnergyTransferRejected:
        return ocpp::v2::ReasonEnum::ReqEnergyTransferRejected;
    case types::evse_manager::StopTransactionReason::SoftReset:
    case types::evse_manager::StopTransactionReason::UnlockCommand:
        return ocpp::v2::ReasonEnum::Other;
    }
    return ocpp::v2::ReasonEnum::Other;
}

ocpp::v2::IdToken to_ocpp_id_token(const types::authorization::IdToken& id_token) {
    ocpp::v2::IdToken ocpp_id_token = {id_token.value, types::authorization::id_token_type_to_string(id_token.type)};
    if (id_token.additional_info.has_value()) {
        std::vector<ocpp::v2::AdditionalInfo> ocpp_additional_info;
        const auto& additional_info = id_token.additional_info.value();
        for (const auto& entry : additional_info) {
            ocpp_additional_info.push_back({entry.value, entry.type});
        }
        ocpp_id_token.additionalInfo = ocpp_additional_info;
    }
    return ocpp_id_token;
}

ocpp::v2::CertificateActionEnum to_ocpp_certificate_action_enum(const types::iso15118::CertificateActionEnum& action) {
    switch (action) {
    case types::iso15118::CertificateActionEnum::Install:
        return ocpp::v2::CertificateActionEnum::Install;
    case types::iso15118::CertificateActionEnum::Update:
        return ocpp::v2::CertificateActionEnum::Update;
    }
    throw std::out_of_range("Could not convert CertificateActionEnum"); // this should never happen
}

types::evse_manager::StopTransactionReason to_everest_stop_transaction_reason(const ocpp::v2::ReasonEnum& stop_reason) {
    switch (stop_reason) {
    case ocpp::v2::ReasonEnum::DeAuthorized:
        return types::evse_manager::StopTransactionReason::DeAuthorized;
    case ocpp::v2::ReasonEnum::EmergencyStop:
        return types::evse_manager::StopTransactionReason::EmergencyStop;
    case ocpp::v2::ReasonEnum::EnergyLimitReached:
        return types::evse_manager::StopTransactionReason::EnergyLimitReached;
    case ocpp::v2::ReasonEnum::EVDisconnected:
        return types::evse_manager::StopTransactionReason::EVDisconnected;
    case ocpp::v2::ReasonEnum::GroundFault:
        return types::evse_manager::StopTransactionReason::GroundFault;
    case ocpp::v2::ReasonEnum::ImmediateReset:
        return types::evse_manager::StopTransactionReason::HardReset;
    case ocpp::v2::ReasonEnum::Local:
        return types::evse_manager::StopTransactionReason::Local;
    case ocpp::v2::ReasonEnum::LocalOutOfCredit:
        return types::evse_manager::StopTransactionReason::LocalOutOfCredit;
    case ocpp::v2::ReasonEnum::MasterPass:
        return types::evse_manager::StopTransactionReason::MasterPass;
    case ocpp::v2::ReasonEnum::Other:
        return types::evse_manager::StopTransactionReason::Other;
    case ocpp::v2::ReasonEnum::OvercurrentFault:
        return types::evse_manager::StopTransactionReason::OvercurrentFault;
    case ocpp::v2::ReasonEnum::PowerLoss:
        return types::evse_manager::StopTransactionReason::PowerLoss;
    case ocpp::v2::ReasonEnum::PowerQuality:
        return types::evse_manager::StopTransactionReason::PowerQuality;
    case ocpp::v2::ReasonEnum::Reboot:
        return types::evse_manager::StopTransactionReason::Reboot;
    case ocpp::v2::ReasonEnum::Remote:
        return types::evse_manager::StopTransactionReason::Remote;
    case ocpp::v2::ReasonEnum::SOCLimitReached:
        return types::evse_manager::StopTransactionReason::SOCLimitReached;
    case ocpp::v2::ReasonEnum::StoppedByEV:
        return types::evse_manager::StopTransactionReason::StoppedByEV;
    case ocpp::v2::ReasonEnum::TimeLimitReached:
        return types::evse_manager::StopTransactionReason::TimeLimitReached;
    case ocpp::v2::ReasonEnum::Timeout:
        return types::evse_manager::StopTransactionReason::Timeout;
    case ocpp::v2::ReasonEnum::ReqEnergyTransferRejected:
        return types::evse_manager::StopTransactionReason::ReqEnergyTransferRejected;
    }
    return types::evse_manager::StopTransactionReason::Other;
}

std::vector<ocpp::v2::OCSPRequestData> to_ocpp_ocsp_request_data_vector(
    const std::vector<types::iso15118::CertificateHashDataInfo>& certificate_hash_data_info) {
    std::vector<ocpp::v2::OCSPRequestData> ocsp_request_data_list;

    for (const auto& certificate_hash_data : certificate_hash_data_info) {
        ocpp::v2::OCSPRequestData ocsp_request_data;
        ocsp_request_data.hashAlgorithm = conversions::to_ocpp_hash_algorithm_enum(certificate_hash_data.hashAlgorithm);
        ocsp_request_data.issuerKeyHash = certificate_hash_data.issuerKeyHash;
        ocsp_request_data.issuerNameHash = certificate_hash_data.issuerNameHash;
        ocsp_request_data.responderURL = certificate_hash_data.responderURL;
        ocsp_request_data.serialNumber = certificate_hash_data.serialNumber;
        ocsp_request_data_list.push_back(ocsp_request_data);
    }
    return ocsp_request_data_list;
}

ocpp::v2::HashAlgorithmEnum to_ocpp_hash_algorithm_enum(const types::iso15118::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::iso15118::HashAlgorithm::SHA256:
        return ocpp::v2::HashAlgorithmEnum::SHA256;
    case types::iso15118::HashAlgorithm::SHA384:
        return ocpp::v2::HashAlgorithmEnum::SHA384;
    case types::iso15118::HashAlgorithm::SHA512:
        return ocpp::v2::HashAlgorithmEnum::SHA512;
    }
    throw std::out_of_range("Could not convert types::iso15118::HashAlgorithm to ocpp::v2::HashAlgorithmEnum");
}

std::vector<ocpp::v2::GetVariableData>
to_ocpp_get_variable_data_vector(const std::vector<types::ocpp::GetVariableRequest>& get_variable_request_vector) {
    std::vector<ocpp::v2::GetVariableData> ocpp_get_variable_data_vector;
    for (const auto& get_variable_request : get_variable_request_vector) {
        ocpp::v2::GetVariableData get_variable_data;
        get_variable_data.component = to_ocpp_component(get_variable_request.component_variable.component);
        get_variable_data.variable = to_ocpp_variable(get_variable_request.component_variable.variable);
        if (get_variable_request.attribute_type.has_value()) {
            get_variable_data.attributeType = to_ocpp_attribute_enum(get_variable_request.attribute_type.value());
        }
        ocpp_get_variable_data_vector.push_back(get_variable_data);
    }
    return ocpp_get_variable_data_vector;
}

std::vector<ocpp::v2::SetVariableData>
to_ocpp_set_variable_data_vector(const std::vector<types::ocpp::SetVariableRequest>& set_variable_request_vector) {
    std::vector<ocpp::v2::SetVariableData> ocpp_set_variable_data_vector;
    for (const auto& set_variable_request : set_variable_request_vector) {
        ocpp::v2::SetVariableData set_variable_data;
        set_variable_data.component = to_ocpp_component(set_variable_request.component_variable.component);
        set_variable_data.variable = to_ocpp_variable(set_variable_request.component_variable.variable);
        if (set_variable_request.attribute_type.has_value()) {
            set_variable_data.attributeType = to_ocpp_attribute_enum(set_variable_request.attribute_type.value());
        }
        try {
            set_variable_data.attributeValue = set_variable_request.value;
        } catch (std::runtime_error& e) {
            EVLOG_error << "Could not convert attributeValue to CiString";
            continue;
        }
        ocpp_set_variable_data_vector.push_back(set_variable_data);
    }
    return ocpp_set_variable_data_vector;
}

ocpp::v2::Component to_ocpp_component(const types::ocpp::Component& component) {
    ocpp::v2::Component _component;
    _component.name = component.name;
    if (component.evse.has_value()) {
        _component.evse = to_ocpp_evse(component.evse.value());
    }
    if (component.instance.has_value()) {
        _component.instance = component.instance.value();
    }
    return _component;
}

ocpp::v2::Variable to_ocpp_variable(const types::ocpp::Variable& variable) {
    ocpp::v2::Variable _variable;
    _variable.name = variable.name;
    if (variable.instance.has_value()) {
        _variable.instance = variable.instance.value();
    }
    return _variable;
}

ocpp::v2::EVSE to_ocpp_evse(const types::ocpp::EVSE& evse) {
    ocpp::v2::EVSE _evse;
    _evse.id = evse.id;
    if (evse.connector_id.has_value()) {
        _evse.connectorId = evse.connector_id.value();
    }
    return _evse;
}

ocpp::v2::AttributeEnum to_ocpp_attribute_enum(const types::ocpp::AttributeEnum attribute_enum) {
    switch (attribute_enum) {
    case types::ocpp::AttributeEnum::Actual:
        return ocpp::v2::AttributeEnum::Actual;
    case types::ocpp::AttributeEnum::Target:
        return ocpp::v2::AttributeEnum::Target;
    case types::ocpp::AttributeEnum::MinSet:
        return ocpp::v2::AttributeEnum::MinSet;
    case types::ocpp::AttributeEnum::MaxSet:
        return ocpp::v2::AttributeEnum::MaxSet;
    }
    throw std::out_of_range("Could not convert AttributeEnum");
}

ocpp::v2::Get15118EVCertificateRequest
to_ocpp_get_15118_certificate_request(const types::iso15118::RequestExiStreamSchema& request) {
    ocpp::v2::Get15118EVCertificateRequest _request;
    _request.iso15118SchemaVersion = request.iso15118_schema_version;
    _request.exiRequest = request.exi_request;
    _request.action = conversions::to_ocpp_certificate_action_enum(request.certificate_action);
    return _request;
}

ocpp::v2::EnergyTransferModeEnum to_ocpp_energy_transfer_mode(const types::iso15118::EnergyTransferMode transfer_mode) {
    switch (transfer_mode) {
    case types::iso15118::EnergyTransferMode::AC_single_phase_core:
        return ocpp::v2::EnergyTransferModeEnum::AC_single_phase;
    case types::iso15118::EnergyTransferMode::AC_two_phase:
        return ocpp::v2::EnergyTransferModeEnum::AC_two_phase;
    case types::iso15118::EnergyTransferMode::AC_three_phase_core:
        return ocpp::v2::EnergyTransferModeEnum::AC_three_phase;
    case types::iso15118::EnergyTransferMode::DC:
    case types::iso15118::EnergyTransferMode::DC_core:
    case types::iso15118::EnergyTransferMode::DC_extended:
    case types::iso15118::EnergyTransferMode::DC_combo_core:
    case types::iso15118::EnergyTransferMode::DC_unique:
        return ocpp::v2::EnergyTransferModeEnum::DC;

    // OCPP 2.1+ enums
    case types::iso15118::EnergyTransferMode::AC_BPT:
        return ocpp::v2::EnergyTransferModeEnum::AC_BPT;
    case types::iso15118::EnergyTransferMode::AC_BPT_DER:
        return ocpp::v2::EnergyTransferModeEnum::AC_BPT_DER;
    case types::iso15118::EnergyTransferMode::AC_DER:
        return ocpp::v2::EnergyTransferModeEnum::AC_DER;
    case types::iso15118::EnergyTransferMode::DC_BPT:
        return ocpp::v2::EnergyTransferModeEnum::DC_BPT;
    case types::iso15118::EnergyTransferMode::DC_ACDP:
        return ocpp::v2::EnergyTransferModeEnum::DC_ACDP;
    case types::iso15118::EnergyTransferMode::DC_ACDP_BPT:
        return ocpp::v2::EnergyTransferModeEnum::DC_ACDP_BPT;
    case types::iso15118::EnergyTransferMode::WPT:
        return ocpp::v2::EnergyTransferModeEnum::WPT;
    case types::iso15118::EnergyTransferMode::MCS:
        return ocpp::v2::EnergyTransferModeEnum::MCS;
    case types::iso15118::EnergyTransferMode::MCS_BPT:
        return ocpp::v2::EnergyTransferModeEnum::MCS_BPT;
    }

    throw std::out_of_range("Could not convert EnergyTransferMode");
}

ocpp::v2::MobilityNeedsModeEnum
to_ocpp_mobility_needs_mode(const types::iso15118::MobilityNeedsMode mobility_needs_mode) {
    switch (mobility_needs_mode) {
    case types::iso15118::MobilityNeedsMode::EVCC:
        return ocpp::v2::MobilityNeedsModeEnum::EVCC;
    case types::iso15118::MobilityNeedsMode::EVCC_SECC:
        return ocpp::v2::MobilityNeedsModeEnum::EVCC_SECC;
    }
    throw std::out_of_range("Could not convert MobilityNeedsMode");
}

ocpp::v2::ControlModeEnum to_ocpp_control_mode(const types::iso15118::ControlMode control_mode) {
    switch (control_mode) {
    case types::iso15118::ControlMode::DynamicControl:
        return ocpp::v2::ControlModeEnum::DynamicControl;
    case types::iso15118::ControlMode::ScheduledControl:
        return ocpp::v2::ControlModeEnum::ScheduledControl;
    }
    throw std::out_of_range("Could not convert ControlMode");
}

ocpp::v2::EVEnergyOffer to_ocpp_ev_energy_offer(const types::iso15118::EVEnergyOffer& ev_energy_offer) {
    ocpp::v2::EVEnergyOffer result;

    result.evPowerSchedule.timeAnchor = ocpp::DateTime(ev_energy_offer.ev_power_schedule.time_anchor);
    result.evPowerSchedule.evPowerScheduleEntries.reserve(
        ev_energy_offer.ev_power_schedule.ev_power_schedule_entries.size());
    for (const auto& entry : ev_energy_offer.ev_power_schedule.ev_power_schedule_entries) {
        ocpp::v2::EVPowerScheduleEntry ocpp_entry;
        ocpp_entry.power = entry.power;
        ocpp_entry.duration = entry.duration;
        result.evPowerSchedule.evPowerScheduleEntries.emplace_back(ocpp_entry);
    }

    if (ev_energy_offer.ev_absolute_price_schedule.has_value()) {
        const auto& iso_absolute_price_schedule = ev_energy_offer.ev_absolute_price_schedule.value();
        ocpp::v2::EVAbsolutePriceSchedule absolute_price_schedule;
        absolute_price_schedule.timeAnchor = ocpp::DateTime(iso_absolute_price_schedule.time_anchor);
        absolute_price_schedule.priceAlgorithm = iso_absolute_price_schedule.price_algorithm;
        absolute_price_schedule.currency = iso_absolute_price_schedule.currency;
        absolute_price_schedule.evAbsolutePriceScheduleEntries.reserve(
            iso_absolute_price_schedule.ev_absolute_price_schedule_entries.size());
        for (const auto& iso_entry : iso_absolute_price_schedule.ev_absolute_price_schedule_entries) {
            ocpp::v2::EVAbsolutePriceScheduleEntry entry;
            entry.duration = iso_entry.duration;
            entry.evPriceRule.reserve(iso_entry.ev_price_rule.size());
            // TODO(mlitre): Transform float to rational number
            for (const auto& iso_rule : iso_entry.ev_price_rule) {
                ocpp::v2::PriceRule rule;
                // rule.energyFee = iso_rule.energy_fee;
                // rule.powerRangeStart = iso_rule.power_range_start;
            }
        }
    }

    return result;
}

ocpp::v2::IslandingDetectionEnum
to_ocpp_islanding_detection(const types::iso15118::IslandingDetection islanding_detection) {
    switch (islanding_detection) {
    case types::iso15118::IslandingDetection::NoAntiIslandingSupport:
        return ocpp::v2::IslandingDetectionEnum::NoAntiIslandingSupport;
    case types::iso15118::IslandingDetection::ZeroCrossingDetection:
        return ocpp::v2::IslandingDetectionEnum::ZeroCrossingDetection;
    case types::iso15118::IslandingDetection::FrequencyJump:
        return ocpp::v2::IslandingDetectionEnum::FrequencyJump;
    case types::iso15118::IslandingDetection::ImpedanceAtFrequency:
        return ocpp::v2::IslandingDetectionEnum::ImpedanceAtFrequency;
    case types::iso15118::IslandingDetection::ImpedanceMeasurement:
        return ocpp::v2::IslandingDetectionEnum::ImpedanceMeasurement;
    case types::iso15118::IslandingDetection::SandiaFrequencyShift:
        return ocpp::v2::IslandingDetectionEnum::SandiaFrequencyShift;
    case types::iso15118::IslandingDetection::SandiaVoltageShift:
        return ocpp::v2::IslandingDetectionEnum::SandiaVoltageShift;
    case types::iso15118::IslandingDetection::SlipModeFrequencyShift:
        return ocpp::v2::IslandingDetectionEnum::SlipModeFrequencyShift;
    case types::iso15118::IslandingDetection::VoltageVectorShift:
        return ocpp::v2::IslandingDetectionEnum::VoltageVectorShift;
    case types::iso15118::IslandingDetection::OtherActive:
        return ocpp::v2::IslandingDetectionEnum::OtherActive;
    case types::iso15118::IslandingDetection::OtherPassive:
        return ocpp::v2::IslandingDetectionEnum::OtherPassive;
    case types::iso15118::IslandingDetection::RCLQFactor:
        return ocpp::v2::IslandingDetectionEnum::RCLQFactor;
    case types::iso15118::IslandingDetection::RoCoF:
        return ocpp::v2::IslandingDetectionEnum::RoCoF;
    case types::iso15118::IslandingDetection::UFP_OFP:
        return ocpp::v2::IslandingDetectionEnum::UFP_OFP;
    case types::iso15118::IslandingDetection::UVP_OVP:
        return ocpp::v2::IslandingDetectionEnum::UVP_OVP;
    }
    throw std::out_of_range("Could not convert IslandingDetection");
}

ocpp::v2::DERControlEnum to_ocpp_der_control(const types::iso15118::DERControl der_control) {
    switch (der_control) {
    case types::iso15118::DERControl::EnterService:
        return ocpp::v2::DERControlEnum::EnterService;
    case types::iso15118::DERControl::FixedPFAbsorb:
        return ocpp::v2::DERControlEnum::FixedPFAbsorb;
    case types::iso15118::DERControl::FixedPFInject:
        return ocpp::v2::DERControlEnum::FixedPFInject;
    case types::iso15118::DERControl::FixedVar:
        return ocpp::v2::DERControlEnum::FixedVar;
    case types::iso15118::DERControl::FreqDroop:
        return ocpp::v2::DERControlEnum::FreqDroop;
    case types::iso15118::DERControl::FreqWatt:
        return ocpp::v2::DERControlEnum::FreqWatt;
    case types::iso15118::DERControl::VoltWatt:
        return ocpp::v2::DERControlEnum::VoltWatt;
    case types::iso15118::DERControl::VoltVar:
        return ocpp::v2::DERControlEnum::VoltVar;
    case types::iso15118::DERControl::Gradients:
        return ocpp::v2::DERControlEnum::Gradients;
    case types::iso15118::DERControl::HFMayTrip:
        return ocpp::v2::DERControlEnum::HFMayTrip;
    case types::iso15118::DERControl::HFMustTrip:
        return ocpp::v2::DERControlEnum::HFMustTrip;
    case types::iso15118::DERControl::HVMustTrip:
        return ocpp::v2::DERControlEnum::HVMustTrip;
    case types::iso15118::DERControl::LFMustTrip:
        return ocpp::v2::DERControlEnum::LFMustTrip;
    case types::iso15118::DERControl::LVMustTrip:
        return ocpp::v2::DERControlEnum::LVMustTrip;
    case types::iso15118::DERControl::PowerMonitoringMustTrip:
        return ocpp::v2::DERControlEnum::PowerMonitoringMustTrip;
    case types::iso15118::DERControl::HVMayTrip:
        return ocpp::v2::DERControlEnum::HVMayTrip;
    case types::iso15118::DERControl::LVMayTrip:
        return ocpp::v2::DERControlEnum::LVMayTrip;
    case types::iso15118::DERControl::HVMomCess:
        return ocpp::v2::DERControlEnum::HVMomCess;
    case types::iso15118::DERControl::LVMomCess:
        return ocpp::v2::DERControlEnum::LVMomCess;
    case types::iso15118::DERControl::LimitMaxDischarge:
        return ocpp::v2::DERControlEnum::LimitMaxDischarge;
    case types::iso15118::DERControl::WattPF:
        return ocpp::v2::DERControlEnum::WattPF;
    case types::iso15118::DERControl::WattVar:
        return ocpp::v2::DERControlEnum::WattVar;
    }
    throw std::out_of_range("Could not convert DERControl");
}

ocpp::v2::ChargingNeeds to_ocpp_charging_needs(const types::iso15118::ChargingNeeds& charging_needs) {
    ocpp::v2::ChargingNeeds _charging_needs;

    _charging_needs.requestedEnergyTransfer = to_ocpp_energy_transfer_mode(charging_needs.requested_energy_transfer);
    if (charging_needs.available_energy_transfer.has_value() and
        not charging_needs.available_energy_transfer->empty()) {
        const auto& available = charging_needs.available_energy_transfer.value();
        _charging_needs.availableEnergyTransfer = {};
        _charging_needs.availableEnergyTransfer->reserve(available.size());
        for (const auto& energy : available) {
            _charging_needs.availableEnergyTransfer->emplace_back(to_ocpp_energy_transfer_mode(energy));
        }
    }
    if (charging_needs.mobility_needs_mode.has_value()) {
        _charging_needs.mobilityNeedsMode = to_ocpp_mobility_needs_mode(charging_needs.mobility_needs_mode.value());
    }
    if (charging_needs.control_mode.has_value()) {
        _charging_needs.controlMode = to_ocpp_control_mode(charging_needs.control_mode.value());
    }
    if (charging_needs.departure_time.has_value()) {
        _charging_needs.departureTime = ocpp::DateTime(charging_needs.departure_time.value());
    }

    if (charging_needs.ac_charging_parameters.has_value()) {
        const auto& ac = charging_needs.ac_charging_parameters.value();
        auto& ac_charging_parameters = _charging_needs.acChargingParameters.emplace();

        ac_charging_parameters.energyAmount = ac.energy_amount;
        ac_charging_parameters.evMinCurrent = ac.ev_min_current;
        ac_charging_parameters.evMaxCurrent = ac.ev_max_current;
        ac_charging_parameters.evMaxVoltage = ac.ev_max_voltage;
    } else if (charging_needs.dc_charging_parameters.has_value()) {
        const auto& dc = charging_needs.dc_charging_parameters.value();
        auto& dc_charging_parameters = _charging_needs.dcChargingParameters.emplace();

        dc_charging_parameters.evMaxCurrent = dc.ev_max_current;
        dc_charging_parameters.evMaxVoltage = dc.ev_max_voltage;
        dc_charging_parameters.energyAmount = dc.energy_amount;
        dc_charging_parameters.evMaxPower = dc.ev_max_power;
        dc_charging_parameters.stateOfCharge = dc.state_of_charge;
        dc_charging_parameters.evEnergyCapacity = dc.ev_energy_capacity;
        dc_charging_parameters.fullSoC = dc.full_soc;
        dc_charging_parameters.bulkSoC = dc.bulk_soc;
    } else if (charging_needs.v2x_charging_parameters.has_value()) {
        const auto& v2x = charging_needs.v2x_charging_parameters.value();
        auto& v2x_charging_params = _charging_needs.v2xChargingParameters.emplace();

        v2x_charging_params.evMaxEnergyRequest = v2x.ev_max_energy_request;
        v2x_charging_params.evMinEnergyRequest = v2x.ev_min_energy_request;
        v2x_charging_params.evMaxV2XEnergyRequest = v2x.ev_max_v2xenergy_request;
        v2x_charging_params.evMinV2XEnergyRequest = v2x.ev_min_v2xenergy_request;
        v2x_charging_params.evTargetEnergyRequest = v2x.ev_target_energy_request;
        v2x_charging_params.targetSoC = v2x.target_soc;
        v2x_charging_params.maxVoltage = v2x.max_voltage;
        v2x_charging_params.minVoltage = v2x.min_voltage;
        v2x_charging_params.maxChargeCurrent = v2x.max_charge_current;
        v2x_charging_params.minChargeCurrent = v2x.min_charge_current;
        v2x_charging_params.maxChargePower = v2x.max_charge_power;
        v2x_charging_params.maxChargePower_L2 = v2x.max_charge_power_l2;
        v2x_charging_params.maxChargePower_L3 = v2x.max_charge_power_l3;
        v2x_charging_params.minChargePower = v2x.min_charge_power;
        v2x_charging_params.minChargePower_L2 = v2x.min_charge_power_l2;
        v2x_charging_params.minChargePower_L3 = v2x.min_charge_power_l3;
        v2x_charging_params.maxDischargeCurrent = v2x.max_discharge_current;
        v2x_charging_params.minDischargeCurrent = v2x.min_discharge_current;
        v2x_charging_params.maxDischargePower = v2x.max_discharge_power;
        v2x_charging_params.maxDischargePower_L2 = v2x.max_discharge_power_l2;
        v2x_charging_params.maxDischargePower_L3 = v2x.max_discharge_power_l3;
        v2x_charging_params.minDischargePower = v2x.min_discharge_power;
        v2x_charging_params.minDischargePower_L2 = v2x.min_discharge_power_l2;
        v2x_charging_params.minDischargePower_L3 = v2x.min_discharge_power_l3;
    }
    if (charging_needs.der_charging_parameters.has_value()) {
        const auto& der = charging_needs.der_charging_parameters.value();
        auto& der_charging_params = _charging_needs.derChargingParameters.emplace();

        der_charging_params.evDurationLevel1DCInjection = der.ev_duration_level1dcinjection;
        der_charging_params.evMaximumLevel1DCInjection = der.ev_maximum_level1dcinjection;
        der_charging_params.evDurationLevel2DCInjection = der.ev_duration_level2dcinjection;
        der_charging_params.evMaximumLevel2DCInjection = der.ev_maximum_level2dcinjection;
        der_charging_params.evIslandingTripTime = der.ev_islanding_trip_time;
        if (der.ev_islanding_detection_method.has_value()) {
            der_charging_params.evIslandingDetectionMethod = {};
            der_charging_params.evIslandingDetectionMethod->reserve(der.ev_islanding_detection_method->size());
            for (const auto& method : der.ev_islanding_detection_method.value()) {
                der_charging_params.evIslandingDetectionMethod->emplace_back(to_ocpp_islanding_detection(method));
            }
        }
        der_charging_params.evInverterHwVersion = der.ev_inverter_hw_version;
        der_charging_params.evInverterSwVersion = der.ev_inverter_sw_version;
        der_charging_params.evInverterManufacturer = der.ev_inverter_manufacturer;
        der_charging_params.evInverterModel = der.ev_inverter_model;
        der_charging_params.evInverterSerialNumber = der.ev_inverter_serial_number;
        der_charging_params.evOverExcitedMaxDischargePower = der.ev_over_excited_max_discharge_power;
        der_charging_params.evOverExcitedPowerFactor = der.ev_over_excited_power_factor;
        der_charging_params.evUnderExcitedMaxDischargePower = der.ev_under_excited_max_discharge_power;
        der_charging_params.evUnderExcitedPowerFactor = der.ev_under_excited_power_factor;
        der_charging_params.maxDischargeApparentPower = der.max_discharge_apparent_power;
        der_charging_params.maxDischargeApparentPower_L2 = der.max_discharge_apparent_power_l2;
        der_charging_params.maxDischargeApparentPower_L3 = der.max_discharge_apparent_power_l3;
        der_charging_params.maxApparentPower = der.max_apparent_power;
        der_charging_params.maxChargeApparentPower = der.max_charge_apparent_power;
        der_charging_params.maxChargeApparentPower_L2 = der.max_charge_apparent_power_l2;
        der_charging_params.maxChargeApparentPower_L3 = der.max_charge_apparent_power_l3;
        der_charging_params.maxChargeReactivePower = der.max_charge_reactive_power;
        der_charging_params.maxChargeReactivePower_L2 = der.max_charge_reactive_power_l2;
        der_charging_params.maxChargeReactivePower_L3 = der.max_charge_reactive_power_l3;
        der_charging_params.maxDischargeReactivePower = der.max_discharge_reactive_power;
        der_charging_params.maxDischargeReactivePower_L2 = der.max_discharge_reactive_power_l2;
        der_charging_params.maxDischargeReactivePower_L3 = der.max_discharge_reactive_power_l3;
        der_charging_params.minDischargeReactivePower = der.min_discharge_reactive_power;
        der_charging_params.minDischargeReactivePower_L2 = der.min_discharge_reactive_power_l2;
        der_charging_params.minDischargeReactivePower_L3 = der.min_discharge_reactive_power_l3;
        der_charging_params.minChargeReactivePower = der.min_charge_reactive_power;
        der_charging_params.minChargeReactivePower_L2 = der.min_charge_reactive_power_l2;
        der_charging_params.minChargeReactivePower_L3 = der.min_charge_reactive_power_l3;
        der_charging_params.evReactiveSusceptance = der.ev_reactive_susceptance;
        der_charging_params.evSessionTotalDischargeEnergyAvailable = der.ev_session_total_discharge_energy_available;
        der_charging_params.maxNominalVoltage = der.max_nominal_voltage;
        der_charging_params.minNominalVoltage = der.min_nominal_voltage;
        der_charging_params.nominalVoltage = der.nominal_voltage;
        der_charging_params.nominalVoltageOffset = der.nominal_voltage_offset;
        if (der.ev_supported_dercontrol.has_value()) {
            der_charging_params.evSupportedDERControl = {};
            der_charging_params.evSupportedDERControl->reserve(der.ev_supported_dercontrol->size());
            for (const auto& der_control : der.ev_supported_dercontrol.value()) {
                der_charging_params.evSupportedDERControl->emplace_back(to_ocpp_der_control(der_control));
            }
        }
    }
    // TODO(mlitre): Support ev energy offer
    // if (charging_needs.ev_energy_offer.has_value()) {
    //  _charging_needs.evEnergyOffer = to_ocpp_ev_energy_offer(charging_needs.ev_energy_offer.value());
    //}

    return _charging_needs;
}

ocpp::v2::ReserveNowStatusEnum to_ocpp_reservation_status(const types::reservation::ReservationResult result) {
    switch (result) {
    case types::reservation::ReservationResult::Accepted:
        return ocpp::v2::ReserveNowStatusEnum::Accepted;
    case types::reservation::ReservationResult::Faulted:
        return ocpp::v2::ReserveNowStatusEnum::Faulted;
    case types::reservation::ReservationResult::Occupied:
        return ocpp::v2::ReserveNowStatusEnum::Occupied;
    case types::reservation::ReservationResult::Rejected:
        return ocpp::v2::ReserveNowStatusEnum::Rejected;
    case types::reservation::ReservationResult::Unavailable:
        return ocpp::v2::ReserveNowStatusEnum::Unavailable;
    }

    throw std::out_of_range("Could not convert ReservationResult");
}

ocpp::v2::ReservationUpdateStatusEnum
to_ocpp_reservation_update_status_enum(const types::reservation::Reservation_status status) {
    switch (status) {
    case types::reservation::Reservation_status::Expired:
        return ocpp::v2::ReservationUpdateStatusEnum::Expired;
    case types::reservation::Reservation_status::Removed:
        return ocpp::v2::ReservationUpdateStatusEnum::Removed;

    case types::reservation::Reservation_status::Cancelled:
    case types::reservation::Reservation_status::Placed:
    case types::reservation::Reservation_status::Used:
        // OCPP should not convert a status enum that is not an OCPP type.
        throw std::out_of_range("Could not convert ReservationUpdateStatus: OCPP does not know this type");
    }

    throw std::out_of_range("Could not convert ReservationUpdateStatus");
}

types::system::UploadLogsRequest to_everest_upload_logs_request(const ocpp::v2::GetLogRequest& request) {
    types::system::UploadLogsRequest _request;
    _request.location = request.log.remoteLocation.get();
    _request.retries = request.retries;
    _request.retry_interval_s = request.retryInterval;

    if (request.log.oldestTimestamp.has_value()) {
        _request.oldest_timestamp = request.log.oldestTimestamp.value().to_rfc3339();
    }
    if (request.log.latestTimestamp.has_value()) {
        _request.latest_timestamp = request.log.latestTimestamp.value().to_rfc3339();
    }
    _request.type = ocpp::v2::conversions::log_enum_to_string(request.logType);
    _request.request_id = request.requestId;
    return _request;
}

types::system::FirmwareUpdateRequest
to_everest_firmware_update_request(const ocpp::v2::UpdateFirmwareRequest& request) {
    types::system::FirmwareUpdateRequest _request;
    _request.request_id = request.requestId;
    _request.location = request.firmware.location.get();
    _request.retries = request.retries;
    _request.retry_interval_s = request.retryInterval;
    _request.retrieve_timestamp = request.firmware.retrieveDateTime.to_rfc3339();
    if (request.firmware.installDateTime.has_value()) {
        _request.install_timestamp = request.firmware.installDateTime.value().to_rfc3339();
    }
    if (request.firmware.signingCertificate.has_value()) {
        _request.signing_certificate = request.firmware.signingCertificate.value().get();
    }
    if (request.firmware.signature.has_value()) {
        _request.signature = request.firmware.signature.value().get();
    }
    return _request;
}

types::iso15118::Status to_everest_iso15118_status(const ocpp::v2::Iso15118EVCertificateStatusEnum& status) {
    switch (status) {
    case ocpp::v2::Iso15118EVCertificateStatusEnum::Accepted:
        return types::iso15118::Status::Accepted;
    case ocpp::v2::Iso15118EVCertificateStatusEnum::Failed:
        return types::iso15118::Status::Failed;
    }
    throw std::out_of_range("Could not convert Iso15118EVCertificateStatusEnum"); // this should never happen
}

types::ocpp::DataTransferStatus to_everest_data_transfer_status(ocpp::v2::DataTransferStatusEnum status) {
    switch (status) {
    case ocpp::v2::DataTransferStatusEnum::Accepted:
        return types::ocpp::DataTransferStatus::Accepted;
    case ocpp::v2::DataTransferStatusEnum::Rejected:
        return types::ocpp::DataTransferStatus::Rejected;
    case ocpp::v2::DataTransferStatusEnum::UnknownMessageId:
        return types::ocpp::DataTransferStatus::UnknownMessageId;
    case ocpp::v2::DataTransferStatusEnum::UnknownVendorId:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    }
    return types::ocpp::DataTransferStatus::UnknownVendorId;
}

types::ocpp::DataTransferRequest to_everest_data_transfer_request(ocpp::v2::DataTransferRequest request) {
    types::ocpp::DataTransferRequest data_transfer_request;
    data_transfer_request.vendor_id = request.vendorId.get();
    if (request.messageId.has_value()) {
        data_transfer_request.message_id = request.messageId.value().get();
    }
    if (request.data.has_value()) {
        data_transfer_request.data = request.data.value().dump();
    }
    if (request.customData.has_value()) {
        auto ocpp_custom_data = request.customData.value();
        types::ocpp::CustomData custom_data{ocpp_custom_data.at("vendorId"), ocpp_custom_data.dump()};
        data_transfer_request.custom_data = custom_data;
    }
    return data_transfer_request;
}

types::ocpp::DataTransferResponse to_everest_data_transfer_response(ocpp::v2::DataTransferResponse response) {
    types::ocpp::DataTransferResponse everest_response;
    everest_response.status = conversions::to_everest_data_transfer_status(response.status);
    if (response.data.has_value()) {
        everest_response.data = response.data.value().dump();
    }
    if (response.customData.has_value()) {
        auto ocpp_custom_data = response.customData.value();
        types::ocpp::CustomData custom_data{ocpp_custom_data.at("vendorId"), ocpp_custom_data.dump()};
        everest_response.custom_data = ocpp_custom_data;
    }
    return everest_response;
}

types::authorization::ValidationResult to_everest_validation_result(const ocpp::v2::IdTokenInfo& idTokenInfo) {
    types::authorization::ValidationResult validation_result;

    validation_result.authorization_status = to_everest_authorization_status(idTokenInfo.status);
    if (idTokenInfo.cacheExpiryDateTime.has_value()) {
        validation_result.expiry_time.emplace(idTokenInfo.cacheExpiryDateTime.value().to_rfc3339());
    }
    if (idTokenInfo.groupIdToken.has_value()) {
        validation_result.parent_id_token = to_everest_id_token(idTokenInfo.groupIdToken.value());
    }

    if (idTokenInfo.personalMessage.has_value()) {
        const types::text_message::MessageContent content =
            to_everest_message_content(idTokenInfo.personalMessage.value());
        validation_result.tariff_messages.push_back(content);
    }

    if (idTokenInfo.customData.has_value() && idTokenInfo.customData.value().contains("vendorId") &&
        idTokenInfo.customData.value().at("vendorId").get<std::string>() == "org.openchargealliance.multilanguage" &&
        idTokenInfo.customData.value().contains("personalMessageExtra")) {
        const json& multi_language_personal_messages = idTokenInfo.customData.value().at("personalMessageExtra");
        for (const auto& messages : multi_language_personal_messages.items()) {
            const types::text_message::MessageContent content = messages.value();
            validation_result.tariff_messages.push_back(content);
        }
    }

    if (idTokenInfo.evseId.has_value()) {
        validation_result.evse_ids = idTokenInfo.evseId.value();
    }
    return validation_result;
}

types::authorization::ValidationResult to_everest_validation_result(const ocpp::v2::AuthorizeResponse& response) {
    types::authorization::ValidationResult validation_result = to_everest_validation_result(response.idTokenInfo);

    if (response.certificateStatus.has_value()) {
        validation_result.certificate_status.emplace(to_everest_certificate_status(response.certificateStatus.value()));
    }

    if (validation_result.allowed_energy_transfer_modes.has_value()) {
        validation_result.allowed_energy_transfer_modes =
            to_everest_allowed_energy_transfer_modes(response.allowedEnergyTransfer.value());
    }
    return validation_result;
}

types::authorization::AuthorizationStatus
to_everest_authorization_status(const ocpp::v2::AuthorizationStatusEnum status) {
    switch (status) {
    case ocpp::v2::AuthorizationStatusEnum::Accepted:
        return types::authorization::AuthorizationStatus::Accepted;
    case ocpp::v2::AuthorizationStatusEnum::Blocked:
        return types::authorization::AuthorizationStatus::Blocked;
    case ocpp::v2::AuthorizationStatusEnum::ConcurrentTx:
        return types::authorization::AuthorizationStatus::ConcurrentTx;
    case ocpp::v2::AuthorizationStatusEnum::Expired:
        return types::authorization::AuthorizationStatus::Expired;
    case ocpp::v2::AuthorizationStatusEnum::Invalid:
        return types::authorization::AuthorizationStatus::Invalid;
    case ocpp::v2::AuthorizationStatusEnum::NoCredit:
        return types::authorization::AuthorizationStatus::NoCredit;
    case ocpp::v2::AuthorizationStatusEnum::NotAllowedTypeEVSE:
        return types::authorization::AuthorizationStatus::NotAllowedTypeEVSE;
    case ocpp::v2::AuthorizationStatusEnum::NotAtThisLocation:
        return types::authorization::AuthorizationStatus::NotAtThisLocation;
    case ocpp::v2::AuthorizationStatusEnum::NotAtThisTime:
        return types::authorization::AuthorizationStatus::NotAtThisTime;
    case ocpp::v2::AuthorizationStatusEnum::Unknown:
        return types::authorization::AuthorizationStatus::Unknown;
    }
    throw std::out_of_range(
        "Could not convert ocpp::v2::AuthorizationStatusEnum to types::authorization::AuthorizationStatus");
}

types::authorization::IdToken to_everest_id_token(const ocpp::v2::IdToken& id_token) {
    types::authorization::IdToken _id_token;
    _id_token.value = id_token.idToken.get();
    _id_token.type = types::authorization::string_to_id_token_type(id_token.type);
    return _id_token;
}

types::authorization::CertificateStatus
to_everest_certificate_status(const ocpp::v2::AuthorizeCertificateStatusEnum status) {
    switch (status) {
    case ocpp::v2::AuthorizeCertificateStatusEnum::Accepted:
        return types::authorization::CertificateStatus::Accepted;
    case ocpp::v2::AuthorizeCertificateStatusEnum::SignatureError:
        return types::authorization::CertificateStatus::SignatureError;
    case ocpp::v2::AuthorizeCertificateStatusEnum::CertificateExpired:
        return types::authorization::CertificateStatus::CertificateExpired;
    case ocpp::v2::AuthorizeCertificateStatusEnum::CertificateRevoked:
        return types::authorization::CertificateStatus::CertificateRevoked;
    case ocpp::v2::AuthorizeCertificateStatusEnum::NoCertificateAvailable:
        return types::authorization::CertificateStatus::NoCertificateAvailable;
    case ocpp::v2::AuthorizeCertificateStatusEnum::CertChainError:
        return types::authorization::CertificateStatus::CertChainError;
    case ocpp::v2::AuthorizeCertificateStatusEnum::ContractCancelled:
        return types::authorization::CertificateStatus::ContractCancelled;
    }
    throw std::out_of_range("Could not convert ocpp::v2::AuthorizeCertificateStatusEnum to "
                            "types::authorization::CertificateStatus");
}

types::ocpp::OcppTransactionEvent
to_everest_ocpp_transaction_event(const ocpp::v2::TransactionEventRequest& transaction_event) {
    types::ocpp::OcppTransactionEvent ocpp_transaction_event;
    switch (transaction_event.eventType) {
    case ocpp::v2::TransactionEventEnum::Started:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Started;
        break;
    case ocpp::v2::TransactionEventEnum::Updated:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Updated;
        break;
    case ocpp::v2::TransactionEventEnum::Ended:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Ended;
        break;
    }

    if (transaction_event.evse.has_value()) {
        ocpp_transaction_event.evse = to_everest_evse(transaction_event.evse.value());
    }
    ocpp_transaction_event.session_id =
        transaction_event.transactionInfo.transactionId; // session_id == transaction_id for OCPP2.0.1
    ocpp_transaction_event.transaction_id = transaction_event.transactionInfo.transactionId;
    return ocpp_transaction_event;
}

types::text_message::MessageFormat to_everest_message_format(const ocpp::v2::MessageFormatEnum& message_format) {
    switch (message_format) {
    case ocpp::v2::MessageFormatEnum::ASCII:
        return types::text_message::MessageFormat::ASCII;
    case ocpp::v2::MessageFormatEnum::HTML:
        return types::text_message::MessageFormat::HTML;
    case ocpp::v2::MessageFormatEnum::URI:
        return types::text_message::MessageFormat::URI;
    case ocpp::v2::MessageFormatEnum::UTF8:
        return types::text_message::MessageFormat::UTF8;
    case ocpp::v2::MessageFormatEnum::QRCODE:
        return types::text_message::MessageFormat::QRCODE;
    }
    throw std::out_of_range("Could not convert ocpp::v2::MessageFormatEnum to types::text_message::MessageFormat");
}

types::text_message::MessageContent to_everest_message_content(const ocpp::v2::MessageContent& message_content) {
    types::text_message::MessageContent everest_message_content;
    everest_message_content.format = to_everest_message_format(message_content.format);
    everest_message_content.content = message_content.content;
    everest_message_content.language = message_content.language;
    return everest_message_content;
}

types::ocpp::OcppTransactionEventResponse
to_everest_transaction_event_response(const ocpp::v2::TransactionEventResponse& transaction_event_response) {
    types::ocpp::OcppTransactionEventResponse everest_transaction_event_response;

    everest_transaction_event_response.total_cost = transaction_event_response.totalCost;
    everest_transaction_event_response.charging_priority = transaction_event_response.chargingPriority;
    if (transaction_event_response.updatedPersonalMessage.has_value()) {
        everest_transaction_event_response.personal_message =
            to_everest_message_content(transaction_event_response.updatedPersonalMessage.value());
    }

    return everest_transaction_event_response;
}

types::ocpp::BootNotificationResponse
to_everest_boot_notification_response(const ocpp::v2::BootNotificationResponse& boot_notification_response) {
    types::ocpp::BootNotificationResponse everest_boot_notification_response;
    everest_boot_notification_response.status = to_everest_registration_status(boot_notification_response.status);
    everest_boot_notification_response.current_time = boot_notification_response.currentTime.to_rfc3339();
    everest_boot_notification_response.interval = boot_notification_response.interval;
    if (boot_notification_response.statusInfo.has_value()) {
        everest_boot_notification_response.status_info =
            to_everest_status_info_type(boot_notification_response.statusInfo.value());
    }
    return everest_boot_notification_response;
}

types::ocpp::RegistrationStatus
to_everest_registration_status(const ocpp::v2::RegistrationStatusEnum& registration_status) {
    switch (registration_status) {
    case ocpp::v2::RegistrationStatusEnum::Accepted:
        return types::ocpp::RegistrationStatus::Accepted;
    case ocpp::v2::RegistrationStatusEnum::Pending:
        return types::ocpp::RegistrationStatus::Pending;
    case ocpp::v2::RegistrationStatusEnum::Rejected:
        return types::ocpp::RegistrationStatus::Rejected;
    }
    throw std::out_of_range("Could not convert ocpp::v2::RegistrationStatusEnum to types::ocpp::RegistrationStatus");
}

types::ocpp::StatusInfoType to_everest_status_info_type(const ocpp::v2::StatusInfo& status_info) {
    types::ocpp::StatusInfoType everest_status_info;
    everest_status_info.reason_code = status_info.reasonCode;
    everest_status_info.additional_info = status_info.additionalInfo;
    return everest_status_info;
}

std::vector<types::ocpp::GetVariableResult>
to_everest_get_variable_result_vector(const std::vector<ocpp::v2::GetVariableResult>& get_variable_result_vector) {
    std::vector<types::ocpp::GetVariableResult> response;
    for (const auto& get_variable_result : get_variable_result_vector) {
        types::ocpp::GetVariableResult _get_variable_result;
        _get_variable_result.status = to_everest_get_variable_status_enum_type(get_variable_result.attributeStatus);
        _get_variable_result.component_variable = {conversions::to_everest_component(get_variable_result.component),
                                                   conversions::to_everest_variable(get_variable_result.variable)};
        if (get_variable_result.attributeType.has_value()) {
            _get_variable_result.attribute_type =
                conversions::to_everest_attribute_enum(get_variable_result.attributeType.value());
        }
        if (get_variable_result.attributeValue.has_value()) {
            _get_variable_result.value = get_variable_result.attributeValue.value().get();
        }
        response.push_back(_get_variable_result);
    }
    return response;
}

std::vector<types::ocpp::SetVariableResult>
to_everest_set_variable_result_vector(const std::vector<ocpp::v2::SetVariableResult>& set_variable_result_vector) {
    std::vector<types::ocpp::SetVariableResult> response;
    for (const auto& set_variable_result : set_variable_result_vector) {
        types::ocpp::SetVariableResult _set_variable_result;
        _set_variable_result.status =
            conversions::to_everest_set_variable_status_enum_type(set_variable_result.attributeStatus);
        _set_variable_result.component_variable = {conversions::to_everest_component(set_variable_result.component),
                                                   conversions::to_everest_variable(set_variable_result.variable)};
        if (set_variable_result.attributeType.has_value()) {
            _set_variable_result.attribute_type =
                conversions::to_everest_attribute_enum(set_variable_result.attributeType.value());
        }
        response.push_back(_set_variable_result);
    }
    return response;
}

types::ocpp::Component to_everest_component(const ocpp::v2::Component& component) {
    types::ocpp::Component _component;
    _component.name = component.name;
    if (component.evse.has_value()) {
        _component.evse = to_everest_evse(component.evse.value());
    }
    if (component.instance.has_value()) {
        _component.instance = component.instance.value();
    }
    return _component;
}

types::ocpp::Variable to_everest_variable(const ocpp::v2::Variable& variable) {
    types::ocpp::Variable _variable;
    _variable.name = variable.name;
    if (variable.instance.has_value()) {
        _variable.instance = variable.instance.value();
    }
    return _variable;
}

types::ocpp::EVSE to_everest_evse(const ocpp::v2::EVSE& evse) {
    types::ocpp::EVSE _evse;
    _evse.id = evse.id;
    if (evse.connectorId.has_value()) {
        _evse.connector_id = evse.connectorId.value();
    }
    return _evse;
}

types::ocpp::AttributeEnum to_everest_attribute_enum(const ocpp::v2::AttributeEnum attribute_enum) {
    switch (attribute_enum) {
    case ocpp::v2::AttributeEnum::Actual:
        return types::ocpp::AttributeEnum::Actual;
    case ocpp::v2::AttributeEnum::Target:
        return types::ocpp::AttributeEnum::Target;
    case ocpp::v2::AttributeEnum::MinSet:
        return types::ocpp::AttributeEnum::MinSet;
    case ocpp::v2::AttributeEnum::MaxSet:
        return types::ocpp::AttributeEnum::MaxSet;
    }
    throw std::out_of_range("Could not convert AttributeEnum");
}

types::ocpp::GetVariableStatusEnumType
to_everest_get_variable_status_enum_type(const ocpp::v2::GetVariableStatusEnum get_variable_status) {
    switch (get_variable_status) {
    case ocpp::v2::GetVariableStatusEnum::Accepted:
        return types::ocpp::GetVariableStatusEnumType::Accepted;
    case ocpp::v2::GetVariableStatusEnum::Rejected:
        return types::ocpp::GetVariableStatusEnumType::Rejected;
    case ocpp::v2::GetVariableStatusEnum::UnknownComponent:
        return types::ocpp::GetVariableStatusEnumType::UnknownComponent;
    case ocpp::v2::GetVariableStatusEnum::UnknownVariable:
        return types::ocpp::GetVariableStatusEnumType::UnknownVariable;
    case ocpp::v2::GetVariableStatusEnum::NotSupportedAttributeType:
        return types::ocpp::GetVariableStatusEnumType::NotSupportedAttributeType;
    }
    throw std::out_of_range("Could not convert GetVariableStatusEnumType");
}

types::ocpp::SetVariableStatusEnumType
to_everest_set_variable_status_enum_type(const ocpp::v2::SetVariableStatusEnum set_variable_status) {
    switch (set_variable_status) {
    case ocpp::v2::SetVariableStatusEnum::Accepted:
        return types::ocpp::SetVariableStatusEnumType::Accepted;
    case ocpp::v2::SetVariableStatusEnum::Rejected:
        return types::ocpp::SetVariableStatusEnumType::Rejected;
    case ocpp::v2::SetVariableStatusEnum::UnknownComponent:
        return types::ocpp::SetVariableStatusEnumType::UnknownComponent;
    case ocpp::v2::SetVariableStatusEnum::UnknownVariable:
        return types::ocpp::SetVariableStatusEnumType::UnknownVariable;
    case ocpp::v2::SetVariableStatusEnum::NotSupportedAttributeType:
        return types::ocpp::SetVariableStatusEnumType::NotSupportedAttributeType;
    case ocpp::v2::SetVariableStatusEnum::RebootRequired:
        return types::ocpp::SetVariableStatusEnumType::RebootRequired;
    }
    throw std::out_of_range("Could not convert GetVariableStatusEnumType");
}

types::ocpp::ChargingSchedules
to_everest_charging_schedules(const std::vector<ocpp::v2::CompositeSchedule>& composite_schedules) {
    types::ocpp::ChargingSchedules charging_schedules;
    for (const auto& composite_schedule : composite_schedules) {
        charging_schedules.schedules.push_back(conversions::to_everest_charging_schedule(composite_schedule));
    }
    return charging_schedules;
}

types::ocpp::ChargingSchedule to_everest_charging_schedule(const ocpp::v2::CompositeSchedule& composite_schedule) {
    types::ocpp::ChargingSchedule charging_schedule;
    charging_schedule.evse = composite_schedule.evseId;
    charging_schedule.charging_rate_unit =
        ocpp::v2::conversions::charging_rate_unit_enum_to_string(composite_schedule.chargingRateUnit);
    charging_schedule.evse = composite_schedule.evseId;
    charging_schedule.duration = composite_schedule.duration;
    charging_schedule.start_schedule = composite_schedule.scheduleStart.to_rfc3339();
    // min_charging_rate is not given as part of a OCPP2.0.1 composite schedule
    for (const auto& charging_schedule_period : composite_schedule.chargingSchedulePeriod) {
        charging_schedule.charging_schedule_period.push_back(
            to_everest_charging_schedule_period(charging_schedule_period));
    }
    return charging_schedule;
}

types::ocpp::Operation_mode to_everest_operation_mode(const ocpp::v2::OperationModeEnum operation_mode) {
    switch (operation_mode) {
    case ocpp::v2::OperationModeEnum::Idle:
        return types::ocpp::Operation_mode::Idle;
    case ocpp::v2::OperationModeEnum::ChargingOnly:
        return types::ocpp::Operation_mode::ChargingOnly;
    case ocpp::v2::OperationModeEnum::CentralSetpoint:
        return types::ocpp::Operation_mode::CentralSetpoint;
    case ocpp::v2::OperationModeEnum::ExternalSetpoint:
        return types::ocpp::Operation_mode::ExternalSetpoint;
    case ocpp::v2::OperationModeEnum::ExternalLimits:
        return types::ocpp::Operation_mode::ExternalLimits;
    case ocpp::v2::OperationModeEnum::CentralFrequency:
        return types::ocpp::Operation_mode::CentralFrequency;
    case ocpp::v2::OperationModeEnum::LocalFrequency:
        return types::ocpp::Operation_mode::LocalFrequency;
    case ocpp::v2::OperationModeEnum::LocalLoadBalancing:
        return types::ocpp::Operation_mode::LocalLoadBalancing;
    }

    throw std::out_of_range("Could not convert operation mode enum.");
}

types::ocpp::ChargingSchedulePeriod
to_everest_charging_schedule_period(const ocpp::v2::ChargingSchedulePeriod& period) {
    if (not period.limit.has_value()) {
        EVLOG_warning << "Received ChargingSchedulePeriod without a limit. Limit defaults to 0!";
    }

    types::ocpp::ChargingSchedulePeriod _period;
    _period.start_period = period.startPeriod;
    _period.limit = period.limit.value_or(0);
    _period.limit_L2 = period.limit_L2;
    _period.limit_L3 = period.limit_L3;
    _period.number_phases = period.numberPhases;
    _period.phase_to_use = period.phaseToUse;
    _period.discharge_limit = period.dischargeLimit;
    _period.discharge_limit_L2 = period.dischargeLimit_L2;
    _period.discharge_limit_L3 = period.dischargeLimit_L3;
    _period.setpoint = period.setpoint;
    _period.setpoint_L2 = period.setpoint_L2;
    _period.setpoint_L3 = period.setpoint_L3;
    _period.setpoint_reactive = period.setpointReactive;
    _period.setpoint_reactive_L2 = period.setpointReactive_L2;
    _period.setpoint_reactive_L3 = period.setpointReactive_L3;
    _period.preconditioning_request = period.preconditioningRequest;
    _period.evse_sleep = period.evseSleep;
    _period.v2x_baseline = period.v2xBaseline;
    if (period.operationMode.has_value()) {
        _period.operation_mode = to_everest_operation_mode(period.operationMode.value());
    }

    if (period.v2xFreqWattCurve.has_value()) {
        _period.v2x_freq_watt_curve = std::vector<types::ocpp::V2XFreqWattPointType>();
        for (const auto& point : period.v2xFreqWattCurve.value()) {
            _period.v2x_freq_watt_curve->push_back({point.frequency, point.power});
        }
    }

    if (period.v2xSignalWattCurve.has_value()) {
        _period.v2x_signal_watt_curve = std::vector<types::ocpp::V2XSignalWattPointCurve>();
        for (const auto& point : period.v2xSignalWattCurve.value()) {
            _period.v2x_signal_watt_curve->push_back({point.signal, point.power});
        }
    }

    return _period;
}

ocpp::v2::DisplayMessageStatusEnum
to_ocpp_display_message_status_enum(const types::display_message::DisplayMessageStatusEnum& from) {
    switch (from) {
    case types::display_message::DisplayMessageStatusEnum::Accepted:
        return ocpp::v2::DisplayMessageStatusEnum::Accepted;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedMessageFormat:
        return ocpp::v2::DisplayMessageStatusEnum::NotSupportedMessageFormat;
    case types::display_message::DisplayMessageStatusEnum::Rejected:
        return ocpp::v2::DisplayMessageStatusEnum::Rejected;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedPriority:
        return ocpp::v2::DisplayMessageStatusEnum::NotSupportedPriority;
    case types::display_message::DisplayMessageStatusEnum::NotSupportedState:
        return ocpp::v2::DisplayMessageStatusEnum::NotSupportedState;
    case types::display_message::DisplayMessageStatusEnum::UnknownTransaction:
        return ocpp::v2::DisplayMessageStatusEnum::UnknownTransaction;
    }

    throw std::out_of_range("Could not convert DisplayMessageStatusEnum");
}

ocpp::v2::SetDisplayMessageResponse
to_ocpp_set_display_message_response(const types::display_message::SetDisplayMessageResponse& response) {
    ocpp::v2::SetDisplayMessageResponse ocpp_response;
    ocpp_response.status = to_ocpp_display_message_status_enum(response.status);
    if (response.status_info.has_value()) {
        ocpp_response.statusInfo = ocpp::v2::StatusInfo();
        ocpp_response.statusInfo.value().additionalInfo = response.status_info.value();
    }

    return ocpp_response;
}

types::display_message::MessagePriorityEnum
to_everest_display_message_priority_enum(const ocpp::v2::MessagePriorityEnum& priority) {
    switch (priority) {
    case ocpp::v2::MessagePriorityEnum::AlwaysFront:
        return types::display_message::MessagePriorityEnum::AlwaysFront;
    case ocpp::v2::MessagePriorityEnum::InFront:
        return types::display_message::MessagePriorityEnum::InFront;
    case ocpp::v2::MessagePriorityEnum::NormalCycle:
        return types::display_message::MessagePriorityEnum::NormalCycle;
    }

    throw std::out_of_range("Could not convert MessagePriorityEnum");
}

types::display_message::MessageStateEnum
to_everest_display_message_state_enum(const ocpp::v2::MessageStateEnum& message_state) {
    switch (message_state) {
    case ocpp::v2::MessageStateEnum::Charging:
        return types::display_message::MessageStateEnum::Charging;
    case ocpp::v2::MessageStateEnum::Faulted:
        return types::display_message::MessageStateEnum::Faulted;
    case ocpp::v2::MessageStateEnum::Idle:
        return types::display_message::MessageStateEnum::Idle;
    case ocpp::v2::MessageStateEnum::Unavailable:
        return types::display_message::MessageStateEnum::Unavailable;
    case ocpp::v2::MessageStateEnum::Suspended:
        return types::display_message::MessageStateEnum::Suspending;
    case ocpp::v2::MessageStateEnum::Discharging:
        return types::display_message::MessageStateEnum::Discharging;
    }

    throw std::out_of_range("Could not convert display message state enum.");
}

types::display_message::GetDisplayMessageRequest
to_everest_display_message_request(const ocpp::v2::GetDisplayMessagesRequest& request) {
    types::display_message::GetDisplayMessageRequest result_request;
    result_request.id = request.id;
    if (request.priority.has_value()) {
        result_request.priority = to_everest_display_message_priority_enum(request.priority.value());
    }
    if (request.state.has_value()) {
        result_request.state = to_everest_display_message_state_enum(request.state.value());
    }

    return result_request;
}

types::display_message::ClearDisplayMessageRequest
to_everest_clear_display_message_request(const ocpp::v2::ClearDisplayMessageRequest& request) {
    types::display_message::ClearDisplayMessageRequest result_request;
    result_request.id = request.id;
    return result_request;
}

ocpp::v2::ClearMessageStatusEnum
to_ocpp_clear_message_response_enum(const types::display_message::ClearMessageResponseEnum& response_enum) {
    switch (response_enum) {
    case types::display_message::ClearMessageResponseEnum::Accepted:
        return ocpp::v2::ClearMessageStatusEnum::Accepted;
    case types::display_message::ClearMessageResponseEnum::Unknown:
        return ocpp::v2::ClearMessageStatusEnum::Unknown;
    }

    throw std::out_of_range("Could not convert ClearMessageResponseEnum");
}

ocpp::v2::ClearDisplayMessageResponse
to_ocpp_clear_display_message_response(const types::display_message::ClearDisplayMessageResponse& response) {
    ocpp::v2::ClearDisplayMessageResponse result_response;
    result_response.status = to_ocpp_clear_message_response_enum(response.status);
    if (response.status_info.has_value()) {
        result_response.statusInfo = ocpp::v2::StatusInfo();
        result_response.statusInfo.value().additionalInfo = response.status_info.value();
    }

    return result_response;
}

types::iso15118::EnergyTransferMode
to_everest_allowed_energy_transfer_mode(const ocpp::v2::EnergyTransferModeEnum& allowed_energy_transfer_mode) {
    switch (allowed_energy_transfer_mode) {
    case ocpp::v2::EnergyTransferModeEnum::AC_BPT:
        return types::iso15118::EnergyTransferMode::AC_BPT;
    case ocpp::v2::EnergyTransferModeEnum::AC_BPT_DER:
        return types::iso15118::EnergyTransferMode::AC_BPT_DER;
    case ocpp::v2::EnergyTransferModeEnum::AC_DER:
        return types::iso15118::EnergyTransferMode::AC_DER;
    case ocpp::v2::EnergyTransferModeEnum::AC_single_phase:
        return types::iso15118::EnergyTransferMode::AC_single_phase_core;
    case ocpp::v2::EnergyTransferModeEnum::AC_three_phase:
        return types::iso15118::EnergyTransferMode::AC_three_phase_core;
    case ocpp::v2::EnergyTransferModeEnum::AC_two_phase:
        return types::iso15118::EnergyTransferMode::AC_two_phase;
    case ocpp::v2::EnergyTransferModeEnum::DC:
        return types::iso15118::EnergyTransferMode::DC;
    case ocpp::v2::EnergyTransferModeEnum::DC_BPT:
        return types::iso15118::EnergyTransferMode::DC_BPT;
    case ocpp::v2::EnergyTransferModeEnum::DC_ACDP:
        return types::iso15118::EnergyTransferMode::DC_ACDP;
    case ocpp::v2::EnergyTransferModeEnum::DC_ACDP_BPT:
        return types::iso15118::EnergyTransferMode::DC_ACDP_BPT;
    case ocpp::v2::EnergyTransferModeEnum::WPT:
        return types::iso15118::EnergyTransferMode::WPT;
    case ocpp::v2::EnergyTransferModeEnum::MCS:
        return types::iso15118::EnergyTransferMode::MCS;
    case ocpp::v2::EnergyTransferModeEnum::MCS_BPT:
        return types::iso15118::EnergyTransferMode::MCS_BPT;
    }
    throw std::out_of_range("Could not convert EnergyTransferModeEnum");
}

std::vector<types::iso15118::EnergyTransferMode> to_everest_allowed_energy_transfer_modes(
    const std::vector<ocpp::v2::EnergyTransferModeEnum>& allowed_energy_transfer_modes) {

    std::vector<types::iso15118::EnergyTransferMode> value{};
    value.reserve(allowed_energy_transfer_modes.size());
    for (const auto& mode : allowed_energy_transfer_modes) {
        value.push_back(to_everest_allowed_energy_transfer_mode(mode));
    }
    return value;
}

} // namespace conversions
} // namespace module
