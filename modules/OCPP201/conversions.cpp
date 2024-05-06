// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <conversions.hpp>
#include <everest/logging.hpp>

namespace module {
namespace conversions {
ocpp::v201::FirmwareStatusEnum to_ocpp_firmware_status_enum(const types::system::FirmwareUpdateStatusEnum status) {
    switch (status) {
    case types::system::FirmwareUpdateStatusEnum::Downloaded:
        return ocpp::v201::FirmwareStatusEnum::Downloaded;
    case types::system::FirmwareUpdateStatusEnum::DownloadFailed:
        return ocpp::v201::FirmwareStatusEnum::DownloadFailed;
    case types::system::FirmwareUpdateStatusEnum::Downloading:
        return ocpp::v201::FirmwareStatusEnum::Downloading;
    case types::system::FirmwareUpdateStatusEnum::DownloadScheduled:
        return ocpp::v201::FirmwareStatusEnum::DownloadScheduled;
    case types::system::FirmwareUpdateStatusEnum::DownloadPaused:
        return ocpp::v201::FirmwareStatusEnum::DownloadPaused;
    case types::system::FirmwareUpdateStatusEnum::Idle:
        return ocpp::v201::FirmwareStatusEnum::Idle;
    case types::system::FirmwareUpdateStatusEnum::InstallationFailed:
        return ocpp::v201::FirmwareStatusEnum::InstallationFailed;
    case types::system::FirmwareUpdateStatusEnum::Installing:
        return ocpp::v201::FirmwareStatusEnum::Installing;
    case types::system::FirmwareUpdateStatusEnum::Installed:
        return ocpp::v201::FirmwareStatusEnum::Installed;
    case types::system::FirmwareUpdateStatusEnum::InstallRebooting:
        return ocpp::v201::FirmwareStatusEnum::InstallRebooting;
    case types::system::FirmwareUpdateStatusEnum::InstallScheduled:
        return ocpp::v201::FirmwareStatusEnum::InstallScheduled;
    case types::system::FirmwareUpdateStatusEnum::InstallVerificationFailed:
        return ocpp::v201::FirmwareStatusEnum::InstallVerificationFailed;
    case types::system::FirmwareUpdateStatusEnum::InvalidSignature:
        return ocpp::v201::FirmwareStatusEnum::InvalidSignature;
    case types::system::FirmwareUpdateStatusEnum::SignatureVerified:
        return ocpp::v201::FirmwareStatusEnum::SignatureVerified;
    default:
        throw std::out_of_range("Could not convert FirmwareUpdateStatusEnum to FirmwareStatusEnum");
    }
}

ocpp::v201::DataTransferStatusEnum to_ocpp_data_transfer_status_enum(types::ocpp::DataTransferStatus status) {
    switch (status) {
    case types::ocpp::DataTransferStatus::Accepted:
        return ocpp::v201::DataTransferStatusEnum::Accepted;
    case types::ocpp::DataTransferStatus::Rejected:
        return ocpp::v201::DataTransferStatusEnum::Rejected;
    case types::ocpp::DataTransferStatus::UnknownMessageId:
        return ocpp::v201::DataTransferStatusEnum::UnknownMessageId;
    case types::ocpp::DataTransferStatus::UnknownVendorId:
        return ocpp::v201::DataTransferStatusEnum::UnknownVendorId;
    default:
        return ocpp::v201::DataTransferStatusEnum::UnknownVendorId;
    }
}

ocpp::v201::DataTransferRequest to_ocpp_data_transfer_request(types::ocpp::DataTransferRequest request) {
    ocpp::v201::DataTransferRequest ocpp_request;
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

ocpp::v201::DataTransferResponse to_ocpp_data_transfer_response(types::ocpp::DataTransferResponse response) {
    ocpp::v201::DataTransferResponse ocpp_response;
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

ocpp::v201::SampledValue to_ocpp_sampled_value(const ocpp::v201::ReadingContextEnum& reading_context,
                                               const ocpp::v201::MeasurandEnum& measurand, const std::string& unit,
                                               const std::optional<ocpp::v201::PhaseEnum> phase) {
    ocpp::v201::SampledValue sampled_value;
    ocpp::v201::UnitOfMeasure unit_of_measure;
    sampled_value.context = reading_context;
    sampled_value.location = ocpp::v201::LocationEnum::Outlet;
    sampled_value.measurand = measurand;
    unit_of_measure.unit = unit;
    sampled_value.unitOfMeasure = unit_of_measure;
    sampled_value.phase = phase;
    return sampled_value;
}

ocpp::v201::SignedMeterValue
to_ocpp_signed_meter_value(const types::units_signed::SignedMeterValue& signed_meter_value) {
    ocpp::v201::SignedMeterValue ocpp_signed_meter_value;
    ocpp_signed_meter_value.signedMeterData = signed_meter_value.signed_meter_data;
    ocpp_signed_meter_value.signingMethod = signed_meter_value.signing_method;
    ocpp_signed_meter_value.encodingMethod = signed_meter_value.encoding_method;
    ocpp_signed_meter_value.publicKey = signed_meter_value.public_key.value_or("");

    return ocpp_signed_meter_value;
}

ocpp::v201::MeterValue
to_ocpp_meter_value(const types::powermeter::Powermeter& power_meter,
                    const ocpp::v201::ReadingContextEnum& reading_context,
                    const std::optional<types::units_signed::SignedMeterValue> signed_meter_value) {
    ocpp::v201::MeterValue meter_value;
    meter_value.timestamp = ocpp::DateTime(power_meter.timestamp);

    // signed_meter_value is intended for OCMF style blobs of signed meter value reports during transaction start or end
    // This is interpreted as Energy.Active.Import.Register
    ocpp::v201::SampledValue sampled_value = to_ocpp_sampled_value(
        reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register, "Wh", std::nullopt);
    sampled_value.value = power_meter.energy_Wh_import.total;
    // add signedMeterValue if present
    if (signed_meter_value.has_value()) {
        sampled_value.signedMeterValue = to_ocpp_signed_meter_value(signed_meter_value.value());
    }
    meter_value.sampledValue.push_back(sampled_value);

    // individual signed meter values can be provided by the power_meter itself

    // Energy.Active.Import.Register
    if (power_meter.energy_Wh_import_signed.has_value()) {
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", std::nullopt);
        const auto& energy_Wh_import_signed = power_meter.energy_Wh_import_signed.value();
        if (energy_Wh_import_signed.total.has_value()) {
            sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_import_signed.total.value());
        }
        meter_value.sampledValue.push_back(sampled_value);
    }

    if (power_meter.energy_Wh_import.L1.has_value()) {
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L1);
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
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L2);
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
        sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Import_Register,
                                              "Wh", ocpp::v201::PhaseEnum::L3);
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
            reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh", std::nullopt);
        sampled_value.value = power_meter.energy_Wh_export.value().total;
        if (power_meter.energy_Wh_export_signed.has_value()) {
            const auto& energy_Wh_export_signed = power_meter.energy_Wh_export_signed.value();
            if (energy_Wh_export_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(energy_Wh_export_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.energy_Wh_export.value().L1.has_value()) {
            sampled_value =
                to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                      ocpp::v201::PhaseEnum::L1);
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
            sampled_value =
                to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                      ocpp::v201::PhaseEnum::L2);
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
            sampled_value =
                to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Energy_Active_Export_Register, "Wh",
                                      ocpp::v201::PhaseEnum::L3);
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
            to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W", std::nullopt);
        sampled_value.value = power_meter.power_W.value().total;
        if (power_meter.power_W_signed.has_value()) {
            const auto& power_W_signed = power_meter.power_W_signed.value();
            if (power_W_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(power_W_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.power_W.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v201::PhaseEnum::L1);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v201::PhaseEnum::L2);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Active_Import, "W",
                                                  ocpp::v201::PhaseEnum::L3);
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
        auto sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import,
                                                   "var", std::nullopt);
        sampled_value.value = power_meter.VAR.value().total;
        if (power_meter.VAR_signed.has_value()) {
            const auto& VAR_signed = power_meter.VAR_signed.value();
            if (VAR_signed.total.has_value()) {
                sampled_value.signedMeterValue = to_ocpp_signed_meter_value(VAR_signed.total.value());
            }
        }
        meter_value.sampledValue.push_back(sampled_value);
        if (power_meter.VAR.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v201::PhaseEnum::L1);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v201::PhaseEnum::L2);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Power_Reactive_Import,
                                                  "var", ocpp::v201::PhaseEnum::L3);
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
            to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A", std::nullopt);
        if (power_meter.current_A.value().L1.has_value()) {
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v201::PhaseEnum::L1);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v201::PhaseEnum::L2);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v201::PhaseEnum::L3);
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
                to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A", std::nullopt);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Current_Import, "A",
                                                  ocpp::v201::PhaseEnum::N);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                                  ocpp::v201::PhaseEnum::L1_N);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                                  ocpp::v201::PhaseEnum::L2_N);
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
            sampled_value = to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V",
                                                  ocpp::v201::PhaseEnum::L3_N);
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
            sampled_value =
                to_ocpp_sampled_value(reading_context, ocpp::v201::MeasurandEnum::Voltage, "V", std::nullopt);
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

ocpp::v201::LogStatusEnum to_ocpp_log_status_enum(types::system::UploadLogsStatus log_status) {
    switch (log_status) {
    case types::system::UploadLogsStatus::Accepted:
        return ocpp::v201::LogStatusEnum::Accepted;
    case types::system::UploadLogsStatus::Rejected:
        return ocpp::v201::LogStatusEnum::Rejected;
    case types::system::UploadLogsStatus::AcceptedCancelled:
        return ocpp::v201::LogStatusEnum::AcceptedCanceled;
    default:
        throw std::runtime_error("Could not convert UploadLogsStatus");
    }
}

ocpp::v201::GetLogResponse to_ocpp_get_log_response(const types::system::UploadLogsResponse& response) {
    ocpp::v201::GetLogResponse _response;
    _response.status = to_ocpp_log_status_enum(response.upload_logs_status);
    _response.filename = response.file_name;
    return _response;
}

ocpp::v201::UpdateFirmwareStatusEnum
to_ocpp_update_firmware_status_enum(const types::system::UpdateFirmwareResponse& response) {
    switch (response) {
    case types::system::UpdateFirmwareResponse::Accepted:
        return ocpp::v201::UpdateFirmwareStatusEnum::Accepted;
    case types::system::UpdateFirmwareResponse::Rejected:
        return ocpp::v201::UpdateFirmwareStatusEnum::Rejected;
    case types::system::UpdateFirmwareResponse::AcceptedCancelled:
        return ocpp::v201::UpdateFirmwareStatusEnum::AcceptedCanceled;
    case types::system::UpdateFirmwareResponse::InvalidCertificate:
        return ocpp::v201::UpdateFirmwareStatusEnum::InvalidCertificate;
    case types::system::UpdateFirmwareResponse::RevokedCertificate:
        return ocpp::v201::UpdateFirmwareStatusEnum::RevokedCertificate;
    default:
        throw std::runtime_error("Could not convert UpdateFirmwareResponse");
    }
}

ocpp::v201::UpdateFirmwareResponse
to_ocpp_update_firmware_response(const types::system::UpdateFirmwareResponse& response) {
    ocpp::v201::UpdateFirmwareResponse _response;
    _response.status = conversions::to_ocpp_update_firmware_status_enum(response);
    return _response;
}

ocpp::v201::UploadLogStatusEnum to_ocpp_upload_logs_status_enum(types::system::LogStatusEnum status) {
    switch (status) {
    case types::system::LogStatusEnum::BadMessage:
        return ocpp::v201::UploadLogStatusEnum::BadMessage;
    case types::system::LogStatusEnum::Idle:
        return ocpp::v201::UploadLogStatusEnum::Idle;
    case types::system::LogStatusEnum::NotSupportedOperation:
        return ocpp::v201::UploadLogStatusEnum::NotSupportedOperation;
    case types::system::LogStatusEnum::PermissionDenied:
        return ocpp::v201::UploadLogStatusEnum::PermissionDenied;
    case types::system::LogStatusEnum::Uploaded:
        return ocpp::v201::UploadLogStatusEnum::Uploaded;
    case types::system::LogStatusEnum::UploadFailure:
        return ocpp::v201::UploadLogStatusEnum::UploadFailure;
    case types::system::LogStatusEnum::Uploading:
        return ocpp::v201::UploadLogStatusEnum::Uploading;
    default:
        throw std::runtime_error("Could not convert UploadLogStatusEnum");
    }
}

ocpp::v201::BootReasonEnum to_ocpp_boot_reason(types::system::BootReason reason) {
    switch (reason) {
    case types::system::BootReason::ApplicationReset:
        return ocpp::v201::BootReasonEnum::ApplicationReset;
    case types::system::BootReason::FirmwareUpdate:
        return ocpp::v201::BootReasonEnum::FirmwareUpdate;
    case types::system::BootReason::LocalReset:
        return ocpp::v201::BootReasonEnum::LocalReset;
    case types::system::BootReason::PowerUp:
        return ocpp::v201::BootReasonEnum::PowerUp;
    case types::system::BootReason::RemoteReset:
        return ocpp::v201::BootReasonEnum::RemoteReset;
    case types::system::BootReason::ScheduledReset:
        return ocpp::v201::BootReasonEnum::ScheduledReset;
    case types::system::BootReason::Triggered:
        return ocpp::v201::BootReasonEnum::Triggered;
    case types::system::BootReason::Unknown:
        return ocpp::v201::BootReasonEnum::Unknown;
    case types::system::BootReason::Watchdog:
        return ocpp::v201::BootReasonEnum::Watchdog;
    default:
        throw std::runtime_error("Could not convert BootReasonEnum");
    }
}

ocpp::v201::ReasonEnum to_ocpp_reason(types::evse_manager::StopTransactionReason reason) {
    switch (reason) {
    case types::evse_manager::StopTransactionReason::DeAuthorized:
        return ocpp::v201::ReasonEnum::DeAuthorized;
    case types::evse_manager::StopTransactionReason::EmergencyStop:
        return ocpp::v201::ReasonEnum::EmergencyStop;
    case types::evse_manager::StopTransactionReason::EnergyLimitReached:
        return ocpp::v201::ReasonEnum::EnergyLimitReached;
    case types::evse_manager::StopTransactionReason::EVDisconnected:
        return ocpp::v201::ReasonEnum::EVDisconnected;
    case types::evse_manager::StopTransactionReason::GroundFault:
        return ocpp::v201::ReasonEnum::GroundFault;
    case types::evse_manager::StopTransactionReason::HardReset:
        return ocpp::v201::ReasonEnum::ImmediateReset;
    case types::evse_manager::StopTransactionReason::Local:
        return ocpp::v201::ReasonEnum::Local;
    case types::evse_manager::StopTransactionReason::LocalOutOfCredit:
        return ocpp::v201::ReasonEnum::LocalOutOfCredit;
    case types::evse_manager::StopTransactionReason::MasterPass:
        return ocpp::v201::ReasonEnum::MasterPass;
    case types::evse_manager::StopTransactionReason::Other:
        return ocpp::v201::ReasonEnum::Other;
    case types::evse_manager::StopTransactionReason::OvercurrentFault:
        return ocpp::v201::ReasonEnum::OvercurrentFault;
    case types::evse_manager::StopTransactionReason::PowerLoss:
        return ocpp::v201::ReasonEnum::PowerLoss;
    case types::evse_manager::StopTransactionReason::PowerQuality:
        return ocpp::v201::ReasonEnum::PowerQuality;
    case types::evse_manager::StopTransactionReason::Reboot:
        return ocpp::v201::ReasonEnum::Reboot;
    case types::evse_manager::StopTransactionReason::Remote:
        return ocpp::v201::ReasonEnum::Remote;
    case types::evse_manager::StopTransactionReason::SOCLimitReached:
        return ocpp::v201::ReasonEnum::SOCLimitReached;
    case types::evse_manager::StopTransactionReason::StoppedByEV:
        return ocpp::v201::ReasonEnum::StoppedByEV;
    case types::evse_manager::StopTransactionReason::TimeLimitReached:
        return ocpp::v201::ReasonEnum::TimeLimitReached;
    case types::evse_manager::StopTransactionReason::Timeout:
        return ocpp::v201::ReasonEnum::Timeout;
    default:
        return ocpp::v201::ReasonEnum::Other;
    }
}

ocpp::v201::IdTokenEnum to_ocpp_id_token_enum(types::authorization::IdTokenType id_token_type) {
    switch (id_token_type) {
    case types::authorization::IdTokenType::Central:
        return ocpp::v201::IdTokenEnum::Central;
    case types::authorization::IdTokenType::eMAID:
        return ocpp::v201::IdTokenEnum::eMAID;
    case types::authorization::IdTokenType::MacAddress:
        return ocpp::v201::IdTokenEnum::MacAddress;
    case types::authorization::IdTokenType::ISO14443:
        return ocpp::v201::IdTokenEnum::ISO14443;
    case types::authorization::IdTokenType::ISO15693:
        return ocpp::v201::IdTokenEnum::ISO15693;
    case types::authorization::IdTokenType::KeyCode:
        return ocpp::v201::IdTokenEnum::KeyCode;
    case types::authorization::IdTokenType::Local:
        return ocpp::v201::IdTokenEnum::Local;
    case types::authorization::IdTokenType::NoAuthorization:
        return ocpp::v201::IdTokenEnum::NoAuthorization;
    default:
        throw std::runtime_error("Could not convert IdTokenEnum");
    }
}

ocpp::v201::IdToken to_ocpp_id_token(const types::authorization::IdToken& id_token) {
    ocpp::v201::IdToken ocpp_id_token = {id_token.value, to_ocpp_id_token_enum(id_token.type)};
    if (id_token.additional_info.has_value()) {
        std::vector<ocpp::v201::AdditionalInfo> ocpp_additional_info;
        const auto& additional_info = id_token.additional_info.value();
        for (const auto& entry : additional_info) {
            ocpp_additional_info.push_back({entry.value, entry.type});
        }
        ocpp_id_token.additionalInfo = ocpp_additional_info;
    }
    return ocpp_id_token;
}

ocpp::v201::CertificateActionEnum
to_ocpp_certificate_action_enum(const types::iso15118_charger::CertificateActionEnum& action) {
    switch (action) {
    case types::iso15118_charger::CertificateActionEnum::Install:
        return ocpp::v201::CertificateActionEnum::Install;
    case types::iso15118_charger::CertificateActionEnum::Update:
        return ocpp::v201::CertificateActionEnum::Update;
    }
    throw std::out_of_range("Could not convert CertificateActionEnum"); // this should never happen
}

types::evse_manager::StopTransactionReason
to_everest_stop_transaction_reason(const ocpp::v201::ReasonEnum& stop_reason) {
    switch (stop_reason) {
    case ocpp::v201::ReasonEnum::DeAuthorized:
        return types::evse_manager::StopTransactionReason::DeAuthorized;
    case ocpp::v201::ReasonEnum::EmergencyStop:
        return types::evse_manager::StopTransactionReason::EmergencyStop;
    case ocpp::v201::ReasonEnum::EnergyLimitReached:
        return types::evse_manager::StopTransactionReason::EnergyLimitReached;
    case ocpp::v201::ReasonEnum::EVDisconnected:
        return types::evse_manager::StopTransactionReason::EVDisconnected;
    case ocpp::v201::ReasonEnum::GroundFault:
        return types::evse_manager::StopTransactionReason::GroundFault;
    case ocpp::v201::ReasonEnum::ImmediateReset:
        return types::evse_manager::StopTransactionReason::HardReset;
    case ocpp::v201::ReasonEnum::Local:
        return types::evse_manager::StopTransactionReason::Local;
    case ocpp::v201::ReasonEnum::LocalOutOfCredit:
        return types::evse_manager::StopTransactionReason::LocalOutOfCredit;
    case ocpp::v201::ReasonEnum::MasterPass:
        return types::evse_manager::StopTransactionReason::MasterPass;
    case ocpp::v201::ReasonEnum::Other:
        return types::evse_manager::StopTransactionReason::Other;
    case ocpp::v201::ReasonEnum::OvercurrentFault:
        return types::evse_manager::StopTransactionReason::OvercurrentFault;
    case ocpp::v201::ReasonEnum::PowerLoss:
        return types::evse_manager::StopTransactionReason::PowerLoss;
    case ocpp::v201::ReasonEnum::PowerQuality:
        return types::evse_manager::StopTransactionReason::PowerQuality;
    case ocpp::v201::ReasonEnum::Reboot:
        return types::evse_manager::StopTransactionReason::Reboot;
    case ocpp::v201::ReasonEnum::Remote:
        return types::evse_manager::StopTransactionReason::Remote;
    case ocpp::v201::ReasonEnum::SOCLimitReached:
        return types::evse_manager::StopTransactionReason::SOCLimitReached;
    case ocpp::v201::ReasonEnum::StoppedByEV:
        return types::evse_manager::StopTransactionReason::StoppedByEV;
    case ocpp::v201::ReasonEnum::TimeLimitReached:
        return types::evse_manager::StopTransactionReason::TimeLimitReached;
    case ocpp::v201::ReasonEnum::Timeout:
        return types::evse_manager::StopTransactionReason::Timeout;
    default:
        return types::evse_manager::StopTransactionReason::Other;
    }
}

std::vector<ocpp::v201::OCSPRequestData> to_ocpp_ocsp_request_data_vector(
    const std::vector<types::iso15118_charger::CertificateHashDataInfo>& certificate_hash_data_info) {
    std::vector<ocpp::v201::OCSPRequestData> ocsp_request_data_list;

    for (const auto& certificate_hash_data : certificate_hash_data_info) {
        ocpp::v201::OCSPRequestData ocsp_request_data;
        ocsp_request_data.hashAlgorithm = conversions::to_ocpp_hash_algorithm_enum(certificate_hash_data.hashAlgorithm);
        ocsp_request_data.issuerKeyHash = certificate_hash_data.issuerKeyHash;
        ocsp_request_data.issuerNameHash = certificate_hash_data.issuerNameHash;
        ocsp_request_data.responderURL = certificate_hash_data.responderURL;
        ocsp_request_data.serialNumber = certificate_hash_data.serialNumber;
        ocsp_request_data_list.push_back(ocsp_request_data);
    }
    return ocsp_request_data_list;
}

ocpp::v201::HashAlgorithmEnum to_ocpp_hash_algorithm_enum(const types::iso15118_charger::HashAlgorithm hash_algorithm) {
    switch (hash_algorithm) {
    case types::iso15118_charger::HashAlgorithm::SHA256:
        return ocpp::v201::HashAlgorithmEnum::SHA256;
    case types::iso15118_charger::HashAlgorithm::SHA384:
        return ocpp::v201::HashAlgorithmEnum::SHA384;
    case types::iso15118_charger::HashAlgorithm::SHA512:
        return ocpp::v201::HashAlgorithmEnum::SHA512;
    default:
        throw std::out_of_range(
            "Could not convert types::iso15118_charger::HashAlgorithm to ocpp::v201::HashAlgorithmEnum");
    }
}

std::vector<ocpp::v201::GetVariableData>
to_ocpp_get_variable_data_vector(const std::vector<types::ocpp::GetVariableRequest>& get_variable_request_vector) {
    std::vector<ocpp::v201::GetVariableData> ocpp_get_variable_data_vector;
    for (const auto& get_variable_request : get_variable_request_vector) {
        ocpp::v201::GetVariableData get_variable_data;
        get_variable_data.component = to_ocpp_component(get_variable_request.component_variable.component);
        get_variable_data.variable = to_ocpp_variable(get_variable_request.component_variable.variable);
        if (get_variable_request.attribute_type.has_value()) {
            get_variable_data.attributeType = to_ocpp_attribute_enum(get_variable_request.attribute_type.value());
        }
        ocpp_get_variable_data_vector.push_back(get_variable_data);
    }
    return ocpp_get_variable_data_vector;
}

std::vector<ocpp::v201::SetVariableData>
to_ocpp_set_variable_data_vector(const std::vector<types::ocpp::SetVariableRequest>& set_variable_request_vector) {
    std::vector<ocpp::v201::SetVariableData> ocpp_set_variable_data_vector;
    for (const auto& set_variable_request : set_variable_request_vector) {
        ocpp::v201::SetVariableData set_variable_data;
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

ocpp::v201::Component to_ocpp_component(const types::ocpp::Component& component) {
    ocpp::v201::Component _component;
    _component.name = component.name;
    if (component.evse.has_value()) {
        _component.evse = to_ocpp_evse(component.evse.value());
    }
    if (component.instance.has_value()) {
        _component.instance = component.instance.value();
    }
    return _component;
}

ocpp::v201::Variable to_ocpp_variable(const types::ocpp::Variable& variable) {
    ocpp::v201::Variable _variable;
    _variable.name = variable.name;
    if (variable.instance.has_value()) {
        _variable.instance = variable.instance.value();
    }
    return _variable;
}

ocpp::v201::EVSE to_ocpp_evse(const types::ocpp::EVSE& evse) {
    ocpp::v201::EVSE _evse;
    _evse.id = evse.id;
    if (evse.connector_id.has_value()) {
        _evse.connectorId = evse.connector_id.value();
    }
    return _evse;
}

ocpp::v201::AttributeEnum to_ocpp_attribute_enum(const types::ocpp::AttributeEnum attribute_enum) {
    switch (attribute_enum) {
    case types::ocpp::AttributeEnum::Actual:
        return ocpp::v201::AttributeEnum::Actual;
    case types::ocpp::AttributeEnum::Target:
        return ocpp::v201::AttributeEnum::Target;
    case types::ocpp::AttributeEnum::MinSet:
        return ocpp::v201::AttributeEnum::MinSet;
    case types::ocpp::AttributeEnum::MaxSet:
        return ocpp::v201::AttributeEnum::MaxSet;
    default:
        throw std::out_of_range("Could not convert AttributeEnum");
    }
}

ocpp::v201::Get15118EVCertificateRequest
to_ocpp_get_15118_certificate_request(const types::iso15118_charger::Request_Exi_Stream_Schema& request) {
    ocpp::v201::Get15118EVCertificateRequest _request;
    _request.iso15118SchemaVersion = request.iso15118SchemaVersion;
    _request.exiRequest = request.exiRequest;
    _request.action = conversions::to_ocpp_certificate_action_enum(request.certificateAction);
    return _request;
}

types::system::UploadLogsRequest to_everest_upload_logs_request(const ocpp::v201::GetLogRequest& request) {
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
    _request.type = ocpp::v201::conversions::log_enum_to_string(request.logType);
    _request.request_id = request.requestId;
    return _request;
}

types::system::FirmwareUpdateRequest
to_everest_firmware_update_request(const ocpp::v201::UpdateFirmwareRequest& request) {
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

types::iso15118_charger::Status
to_everest_iso15118_charger_status(const ocpp::v201::Iso15118EVCertificateStatusEnum& status) {
    switch (status) {
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Accepted:
        return types::iso15118_charger::Status::Accepted;
    case ocpp::v201::Iso15118EVCertificateStatusEnum::Failed:
        return types::iso15118_charger::Status::Failed;
    }
    throw std::out_of_range("Could not convert Iso15118EVCertificateStatusEnum"); // this should never happen
}

types::ocpp::DataTransferStatus to_everest_data_transfer_status(ocpp::v201::DataTransferStatusEnum status) {
    switch (status) {
    case ocpp::v201::DataTransferStatusEnum::Accepted:
        return types::ocpp::DataTransferStatus::Accepted;
    case ocpp::v201::DataTransferStatusEnum::Rejected:
        return types::ocpp::DataTransferStatus::Rejected;
    case ocpp::v201::DataTransferStatusEnum::UnknownMessageId:
        return types::ocpp::DataTransferStatus::UnknownMessageId;
    case ocpp::v201::DataTransferStatusEnum::UnknownVendorId:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    default:
        return types::ocpp::DataTransferStatus::UnknownVendorId;
    }
}

types::ocpp::DataTransferRequest to_everest_data_transfer_request(ocpp::v201::DataTransferRequest request) {
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

types::ocpp::DataTransferResponse to_everest_data_transfer_response(ocpp::v201::DataTransferResponse response) {
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

types::authorization::ValidationResult to_everest_validation_result(const ocpp::v201::AuthorizeResponse& response) {
    types::authorization::ValidationResult validation_result;

    validation_result.authorization_status = to_everest_authorization_status(response.idTokenInfo.status);
    if (response.idTokenInfo.cacheExpiryDateTime.has_value()) {
        validation_result.expiry_time.emplace(response.idTokenInfo.cacheExpiryDateTime.value().to_rfc3339());
    }
    if (response.idTokenInfo.groupIdToken.has_value()) {
        validation_result.parent_id_token = to_everest_id_token(response.idTokenInfo.groupIdToken.value());
    }
    if (response.idTokenInfo.personalMessage.has_value()) {
        validation_result.reason.emplace(response.idTokenInfo.personalMessage.value().content.get());
    }
    if (response.certificateStatus.has_value()) {
        validation_result.certificate_status.emplace(to_everest_certificate_status(response.certificateStatus.value()));
    }
    if (response.idTokenInfo.evseId.has_value()) {
        validation_result.evse_ids = response.idTokenInfo.evseId.value();
    }
    return validation_result;
}

types::authorization::AuthorizationStatus
to_everest_authorization_status(const ocpp::v201::AuthorizationStatusEnum status) {
    switch (status) {
    case ocpp::v201::AuthorizationStatusEnum::Accepted:
        return types::authorization::AuthorizationStatus::Accepted;
    case ocpp::v201::AuthorizationStatusEnum::Blocked:
        return types::authorization::AuthorizationStatus::Blocked;
    case ocpp::v201::AuthorizationStatusEnum::ConcurrentTx:
        return types::authorization::AuthorizationStatus::ConcurrentTx;
    case ocpp::v201::AuthorizationStatusEnum::Expired:
        return types::authorization::AuthorizationStatus::Expired;
    case ocpp::v201::AuthorizationStatusEnum::Invalid:
        return types::authorization::AuthorizationStatus::Invalid;
    case ocpp::v201::AuthorizationStatusEnum::NoCredit:
        return types::authorization::AuthorizationStatus::NoCredit;
    case ocpp::v201::AuthorizationStatusEnum::NotAllowedTypeEVSE:
        return types::authorization::AuthorizationStatus::NotAllowedTypeEVSE;
    case ocpp::v201::AuthorizationStatusEnum::NotAtThisLocation:
        return types::authorization::AuthorizationStatus::NotAtThisLocation;
    case ocpp::v201::AuthorizationStatusEnum::NotAtThisTime:
        return types::authorization::AuthorizationStatus::NotAtThisTime;
    case ocpp::v201::AuthorizationStatusEnum::Unknown:
        return types::authorization::AuthorizationStatus::Unknown;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::AuthorizationStatusEnum to types::authorization::AuthorizationStatus");
    }
}

types::authorization::IdTokenType to_everest_id_token_type(const ocpp::v201::IdTokenEnum& type) {
    switch (type) {
    case ocpp::v201::IdTokenEnum::Central:
        return types::authorization::IdTokenType::Central;
    case ocpp::v201::IdTokenEnum::eMAID:
        return types::authorization::IdTokenType::eMAID;
    case ocpp::v201::IdTokenEnum::ISO14443:
        return types::authorization::IdTokenType::ISO14443;
    case ocpp::v201::IdTokenEnum::ISO15693:
        return types::authorization::IdTokenType::ISO15693;
    case ocpp::v201::IdTokenEnum::KeyCode:
        return types::authorization::IdTokenType::KeyCode;
    case ocpp::v201::IdTokenEnum::Local:
        return types::authorization::IdTokenType::Local;
    case ocpp::v201::IdTokenEnum::MacAddress:
        return types::authorization::IdTokenType::MacAddress;
    case ocpp::v201::IdTokenEnum::NoAuthorization:
        return types::authorization::IdTokenType::NoAuthorization;
    default:
        throw std::out_of_range("Could not convert ocpp::v201::IdTokenEnum to types::authorization::IdTokenType");
    }
}

types::authorization::IdToken to_everest_id_token(const ocpp::v201::IdToken& id_token) {
    types::authorization::IdToken _id_token;
    _id_token.value = id_token.idToken.get();
    _id_token.type = to_everest_id_token_type(id_token.type);
    return _id_token;
}

types::authorization::CertificateStatus
to_everest_certificate_status(const ocpp::v201::AuthorizeCertificateStatusEnum status) {
    switch (status) {
    case ocpp::v201::AuthorizeCertificateStatusEnum::Accepted:
        return types::authorization::CertificateStatus::Accepted;
    case ocpp::v201::AuthorizeCertificateStatusEnum::SignatureError:
        return types::authorization::CertificateStatus::SignatureError;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertificateExpired:
        return types::authorization::CertificateStatus::CertificateExpired;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertificateRevoked:
        return types::authorization::CertificateStatus::CertificateRevoked;
    case ocpp::v201::AuthorizeCertificateStatusEnum::NoCertificateAvailable:
        return types::authorization::CertificateStatus::NoCertificateAvailable;
    case ocpp::v201::AuthorizeCertificateStatusEnum::CertChainError:
        return types::authorization::CertificateStatus::CertChainError;
    case ocpp::v201::AuthorizeCertificateStatusEnum::ContractCancelled:
        return types::authorization::CertificateStatus::ContractCancelled;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::AuthorizeCertificateStatusEnum to types::authorization::CertificateStatus");
    }
}

types::ocpp::OcppTransactionEvent
to_everest_ocpp_transaction_event(const ocpp::v201::TransactionEventRequest& transaction_event) {
    types::ocpp::OcppTransactionEvent ocpp_transaction_event;
    switch (transaction_event.eventType) {
    case ocpp::v201::TransactionEventEnum::Started:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Started;
        break;
    case ocpp::v201::TransactionEventEnum::Updated:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Updated;
        break;
    case ocpp::v201::TransactionEventEnum::Ended:
        ocpp_transaction_event.transaction_event = types::ocpp::TransactionEvent::Ended;
        break;
    }

    auto evse_id = 1;
    auto connector_id = 1;

    if (transaction_event.evse.has_value()) {
        evse_id = transaction_event.evse.value().id;
        if (transaction_event.evse.value().connectorId.has_value()) {
            connector_id = transaction_event.evse.value().connectorId.value();
        }
    } else {
        EVLOG_warning << "Attempting to convert TransactionEventRequest that does not contain information about the "
                         "EVSE. evse_id and connector default to 1.";
    }

    ocpp_transaction_event.evse_id = evse_id;
    ocpp_transaction_event.connector = connector_id;
    ocpp_transaction_event.session_id =
        transaction_event.transactionInfo.transactionId; // session_id == transaction_id for OCPP2.0.1
    ocpp_transaction_event.transaction_id = transaction_event.transactionInfo.transactionId;
    return ocpp_transaction_event;
}

types::ocpp::MessageFormat to_everest_message_format(const ocpp::v201::MessageFormatEnum& message_format) {
    switch (message_format) {
    case ocpp::v201::MessageFormatEnum::ASCII:
        return types::ocpp::MessageFormat::ASCII;
    case ocpp::v201::MessageFormatEnum::HTML:
        return types::ocpp::MessageFormat::HTML;
    case ocpp::v201::MessageFormatEnum::URI:
        return types::ocpp::MessageFormat::URI;
    case ocpp::v201::MessageFormatEnum::UTF8:
        return types::ocpp::MessageFormat::UTF8;
    default:
        throw std::out_of_range("Could not convert ocpp::v201::MessageFormatEnum to types::ocpp::MessageFormat");
    }
}

types::ocpp::MessageContent to_everest_message_content(const ocpp::v201::MessageContent& message_content) {
    types::ocpp::MessageContent everest_message_content;
    everest_message_content.format = to_everest_message_format(message_content.format);
    everest_message_content.content = message_content.content;
    everest_message_content.language = message_content.language;
    return everest_message_content;
}

types::ocpp::OcppTransactionEventResponse
to_everest_transaction_event_response(const ocpp::v201::TransactionEventResponse& transaction_event_response) {
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
to_everest_boot_notification_response(const ocpp::v201::BootNotificationResponse& boot_notification_response) {
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
to_everest_registration_status(const ocpp::v201::RegistrationStatusEnum& registration_status) {
    switch (registration_status) {
    case ocpp::v201::RegistrationStatusEnum::Accepted:
        return types::ocpp::RegistrationStatus::Accepted;
    case ocpp::v201::RegistrationStatusEnum::Pending:
        return types::ocpp::RegistrationStatus::Pending;
    case ocpp::v201::RegistrationStatusEnum::Rejected:
        return types::ocpp::RegistrationStatus::Rejected;
    default:
        throw std::out_of_range(
            "Could not convert ocpp::v201::RegistrationStatusEnum to types::ocpp::RegistrationStatus");
    }
}

types::ocpp::StatusInfoType to_everest_status_info_type(const ocpp::v201::StatusInfo& status_info) {
    types::ocpp::StatusInfoType everest_status_info;
    everest_status_info.reason_code = status_info.reasonCode;
    everest_status_info.additional_info = status_info.additionalInfo;
    return everest_status_info;
}

std::vector<types::ocpp::GetVariableResult>
to_everest_get_variable_result_vector(const std::vector<ocpp::v201::GetVariableResult>& get_variable_result_vector) {
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
to_everest_set_variable_result_vector(const std::vector<ocpp::v201::SetVariableResult>& set_variable_result_vector) {
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

types::ocpp::Component to_everest_component(const ocpp::v201::Component& component) {
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

types::ocpp::Variable to_everest_variable(const ocpp::v201::Variable& variable) {
    types::ocpp::Variable _variable;
    _variable.name = variable.name;
    if (variable.instance.has_value()) {
        _variable.instance = variable.instance.value();
    }
    return _variable;
}

types::ocpp::EVSE to_everest_evse(const ocpp::v201::EVSE& evse) {
    types::ocpp::EVSE _evse;
    _evse.id = evse.id;
    if (evse.connectorId.has_value()) {
        _evse.connector_id = evse.connectorId.value();
    }
    return _evse;
}

types::ocpp::AttributeEnum to_everest_attribute_enum(const ocpp::v201::AttributeEnum attribute_enum) {
    switch (attribute_enum) {
    case ocpp::v201::AttributeEnum::Actual:
        return types::ocpp::AttributeEnum::Actual;
    case ocpp::v201::AttributeEnum::Target:
        return types::ocpp::AttributeEnum::Target;
    case ocpp::v201::AttributeEnum::MinSet:
        return types::ocpp::AttributeEnum::MinSet;
    case ocpp::v201::AttributeEnum::MaxSet:
        return types::ocpp::AttributeEnum::MaxSet;
    default:
        throw std::out_of_range("Could not convert AttributeEnum");
    }
}

types::ocpp::GetVariableStatusEnumType
to_everest_get_variable_status_enum_type(const ocpp::v201::GetVariableStatusEnum get_variable_status) {
    switch (get_variable_status) {
    case ocpp::v201::GetVariableStatusEnum::Accepted:
        return types::ocpp::GetVariableStatusEnumType::Accepted;
    case ocpp::v201::GetVariableStatusEnum::Rejected:
        return types::ocpp::GetVariableStatusEnumType::Rejected;
    case ocpp::v201::GetVariableStatusEnum::UnknownComponent:
        return types::ocpp::GetVariableStatusEnumType::UnknownComponent;
    case ocpp::v201::GetVariableStatusEnum::UnknownVariable:
        return types::ocpp::GetVariableStatusEnumType::UnknownVariable;
    case ocpp::v201::GetVariableStatusEnum::NotSupportedAttributeType:
        return types::ocpp::GetVariableStatusEnumType::NotSupportedAttributeType;
    default:
        throw std::out_of_range("Could not convert GetVariableStatusEnumType");
    }
}

types::ocpp::SetVariableStatusEnumType
to_everest_set_variable_status_enum_type(const ocpp::v201::SetVariableStatusEnum set_variable_status) {
    switch (set_variable_status) {
    case ocpp::v201::SetVariableStatusEnum::Accepted:
        return types::ocpp::SetVariableStatusEnumType::Accepted;
    case ocpp::v201::SetVariableStatusEnum::Rejected:
        return types::ocpp::SetVariableStatusEnumType::Rejected;
    case ocpp::v201::SetVariableStatusEnum::UnknownComponent:
        return types::ocpp::SetVariableStatusEnumType::UnknownComponent;
    case ocpp::v201::SetVariableStatusEnum::UnknownVariable:
        return types::ocpp::SetVariableStatusEnumType::UnknownVariable;
    case ocpp::v201::SetVariableStatusEnum::NotSupportedAttributeType:
        return types::ocpp::SetVariableStatusEnumType::NotSupportedAttributeType;
    case ocpp::v201::SetVariableStatusEnum::RebootRequired:
        return types::ocpp::SetVariableStatusEnumType::RebootRequired;
    default:
        throw std::out_of_range("Could not convert GetVariableStatusEnumType");
    }
}

} // namespace conversions
} // namespace module
