// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <conversions.hpp>
#include <everest/logging.hpp>
#include <ocpp_conversions.hpp>

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
    _response.filename = response.file_name;
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

    case types::iso15118::EnergyTransferMode::AC_BPT:
    case types::iso15118::EnergyTransferMode::AC_BPT_DER:
    case types::iso15118::EnergyTransferMode::AC_DER:
    case types::iso15118::EnergyTransferMode::DC_BPT:
    case types::iso15118::EnergyTransferMode::DC_ACDP:
    case types::iso15118::EnergyTransferMode::DC_ACDP_BPT:
    case types::iso15118::EnergyTransferMode::WPT:
        throw std::out_of_range("Could not convert EnergyTransferModeEnum: OCPP does not know this type");
    }

    throw std::out_of_range("Could not convert EnergyTransferMode");
}

ocpp::v2::NotifyEVChargingNeedsRequest
to_ocpp_notify_ev_charging_needs_request(const types::iso15118::ChargingNeeds& charging_needs) {
    // The evseId is calculated outside of this function in the required OCPP201 module
    ocpp::v2::NotifyEVChargingNeedsRequest _request;
    ocpp::v2::ChargingNeeds& _charging_needs = _request.chargingNeeds;

    _charging_needs.requestedEnergyTransfer = to_ocpp_energy_transfer_mode(charging_needs.requested_energy_transfer);

    // TODO(ioan): add v2x params
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
    }

    return _request;
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

types::authorization::ValidationResult to_everest_validation_result(const ocpp::v2::AuthorizeResponse& response) {
    types::authorization::ValidationResult validation_result;

    validation_result.authorization_status = to_everest_authorization_status(response.idTokenInfo.status);
    if (response.idTokenInfo.cacheExpiryDateTime.has_value()) {
        validation_result.expiry_time.emplace(response.idTokenInfo.cacheExpiryDateTime.value().to_rfc3339());
    }
    if (response.idTokenInfo.groupIdToken.has_value()) {
        validation_result.parent_id_token = to_everest_id_token(response.idTokenInfo.groupIdToken.value());
    }

    if (response.idTokenInfo.personalMessage.has_value()) {
        validation_result.reason = types::authorization::TokenValidationStatusMessage();
        validation_result.reason->messages = std::vector<types::display_message::MessageContent>();
        const types::display_message::MessageContent content =
            to_everest_message_content(response.idTokenInfo.personalMessage.value());
        validation_result.reason->messages->push_back(content);
    }

    if (response.idTokenInfo.customData.has_value() && response.idTokenInfo.customData.value().contains("vendorId") &&
        response.idTokenInfo.customData.value().at("vendorId").get<std::string>() ==
            "org.openchargealliance.multilanguage" &&
        response.idTokenInfo.customData.value().contains("personalMessageExtra")) {
        if (!validation_result.reason->messages.has_value()) {
            validation_result.reason->messages = std::vector<types::display_message::MessageContent>();
        }

        const json& multi_language_personal_messages =
            response.idTokenInfo.customData.value().at("personalMessageExtra");
        for (const auto& messages : multi_language_personal_messages.items()) {
            const types::display_message::MessageContent content = messages.value();
            validation_result.reason->messages->push_back(content);
        }
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

types::display_message::MessageFormat to_everest_message_format(const ocpp::v2::MessageFormatEnum& message_format) {
    switch (message_format) {
    case ocpp::v2::MessageFormatEnum::ASCII:
        return types::display_message::MessageFormat::ASCII;
    case ocpp::v2::MessageFormatEnum::HTML:
        return types::display_message::MessageFormat::HTML;
    case ocpp::v2::MessageFormatEnum::URI:
        return types::display_message::MessageFormat::URI;
    case ocpp::v2::MessageFormatEnum::UTF8:
        return types::display_message::MessageFormat::UTF8;
    case ocpp::v2::MessageFormatEnum::QRCODE:
        return types::display_message::MessageFormat::QRCODE;
    }
    throw std::out_of_range("Could not convert ocpp::v2::MessageFormatEnum to types::ocpp::MessageFormat");
}

types::display_message::MessageContent to_everest_message_content(const ocpp::v2::MessageContent& message_content) {
    types::display_message::MessageContent everest_message_content;
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

types::ocpp::ChargingSchedulePeriod
to_everest_charging_schedule_period(const ocpp::v2::ChargingSchedulePeriod& period) {
    if (not period.limit.has_value()) {
        EVLOG_warning << "Received ChargingSchedulePeriod without a limit. Limit defaults to 0!";
    }

    types::ocpp::ChargingSchedulePeriod _period;
    _period.start_period = period.startPeriod;
    _period.limit = period.limit.value_or(0);
    _period.number_phases = period.numberPhases;
    _period.phase_to_use = period.phaseToUse;
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

} // namespace conversions
} // namespace module
