// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "isabellenhuette_IemDcr_controller.hpp"
#include <stdexcept>
namespace module::main {

json IsaIemDcrController::get_gw() {
	const std::string endpoint = "/counter/v1/ocmf/gw";
	auto response = this->http_client->get(endpoint);
	if (response.status_code == 200) {
		try {
			json data = json::parse(response.body);
			return data;
		} catch (json::exception& json_error) {
			throw UnexpectedIemDcrResponseBody(
            endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
		}
	} else {
		throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
	}	
}

void IsaIemDcrController::post_gw() {
    const std::string endpoint = "/counter/v1/ocmf/gw";
    const std::string payload = nlohmann::ordered_json{{"CT", snapshotConfig.CT},
                                      {"CI", snapshotConfig.CI},
                                      {"TM", helper_get_current_datetime()}}
									  .dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

void IsaIemDcrController::post_tariff(std::string tariffInfo) {
    const std::string endpoint = "/counter/v1/ocmf/tariff";
    const std::string payload = nlohmann::ordered_json{{"TT", tariffInfo}}
									  .dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

std::tuple<types::powermeter::Powermeter, std::string, bool> IsaIemDcrController::get_metervalue() {
	const std::string endpoint = "/counter/v1/ocmf/metervalue";
	auto response = this->http_client->get(endpoint);
	if (response.status_code != 200) {
		throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
	}
	try {
		json data = json::parse(response.body);
		
		types::powermeter::Powermeter powermeter;
		bool transactionActive = data.at("XT");
		powermeter.timestamp = data.at("TM");
		//Remove format specifier at the end (if available)
		if(powermeter.timestamp.length() > 28) {
			powermeter.timestamp = powermeter.timestamp.substr(0, 28);
		}
		powermeter.meter_id = data.at("MS");
		auto current = types::units::Current{};
		current.DC = data.at("I");
		powermeter.current_A.emplace(current);
		auto voltageU2 = types::units::Voltage{};
		voltageU2.DC = data.at("U2");
		powermeter.voltage_V.emplace(voltageU2);
		powermeter.power_W.emplace(types::units::Power{data.at("P").get<float>()});
		//Remove quotes before casting to float
		auto energy_kWh_import = helper_remove_first_and_last_char(data.at("RD").at(2).at("WV"));
		powermeter.energy_Wh_import = {std::stof(energy_kWh_import) * 1000.0f};
		//Remove quotes before casting to float
		auto energy_kWh_export = helper_remove_first_and_last_char(data.at("RD").at(3).at("WV"));
		powermeter.energy_Wh_export = {std::stof(energy_kWh_export) * 1000.0f};
		//Get status
		std::string status = data.at("XC");
		
		return std::make_tuple(powermeter, status, transactionActive);
	} catch (json::exception& json_error) {
		throw UnexpectedIemDcrResponseBody(
            endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
	}
}

std::string IsaIemDcrController::get_publickey(bool allowCachedValue) {
	if(allowCachedValue && cachedPublicKey.length() > 0) {
		return cachedPublicKey;
	} else {
		const std::string endpoint = "/counter/v1/ocmf/publickey";
		auto response = this->http_client->get(endpoint);
		if (response.status_code != 200) {
			EVLOG_warning << "Response is not 200." << std::endl;
			return "";
		}
		try {
			json data = json::parse(response.body);
			cachedPublicKey = data.at("PK");
			return cachedPublicKey;
		} catch (json::exception& json_error) {
			EVLOG_warning << "JSON error" << std::endl;
			return "";
		}
	}
}
	
std::string IsaIemDcrController::get_datetime() {
	const std::string endpoint = "/counter/v1/ocmf/datetime";
	auto response = this->http_client->get(endpoint);
	if (response.status_code != 200) {
		throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
	}
	try {
		json data = json::parse(response.body);
		return data.at("TM");
	} catch (json::exception& json_error) {
		throw UnexpectedIemDcrResponseBody(
            endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
	}
}

void IsaIemDcrController::post_datetime() {
    const std::string endpoint = "/counter/v1/ocmf/datetime";
    const std::string payload = nlohmann::ordered_json{{"TM", helper_get_current_datetime()}}
									  .dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

void IsaIemDcrController::post_user(const types::powermeter::OCMFUserIdentificationStatus IS, 
										const std::optional<types::powermeter::OCMFIdentificationLevel> IL, 
										const std::vector<types::powermeter::OCMFIdentificationFlags> IF,
										const types::powermeter::OCMFIdentificationType& IT,
										const std::optional<std::__cxx11::basic_string<char>>& ID,
										const std::optional<std::__cxx11::basic_string<char>>& TT) {
										
    const std::string endpoint 	= "/counter/v1/ocmf/user";
    bool boolIS 				= helper_get_bool_from_OCMFUserIdentificationStatus(IS);
    std::string strIL 			= helper_get_string_from_OCMFIdentificationLevel(IL);
    std::string strIT			= helper_get_string_from_OCMFIdentificationType(IT);
    std::string strID			= static_cast<std::string>(ID.value_or(""));
    std::string strTT			= static_cast<std::string>(TT.value_or(""));
    std::string payload 		= "";
    std::vector<std::string> vectIF;
    
    //Fill vectIF
    for (const types::powermeter::OCMFIdentificationFlags& idFlag : IF) {
        vectIF.push_back(helper_get_string_from_OCMFIdentificationFlags(idFlag));
    }
    
    if(strTT.length() > 0) {
		payload = nlohmann::ordered_json{{"IS", boolIS},
                                      {"IL", strIL},
                                      {"IF", vectIF},
                                      {"IT", strIT},
                                      {"ID", strID},
                                      {"US", snapshotConfig.US},
                                      {"TT", strTT}}
									  .dump();
	} else {
		payload = nlohmann::ordered_json{{"IS", boolIS},
                                      {"IL", strIL},
                                      {"IF", vectIF},
                                      {"IT", strIT},
                                      {"ID", strID},
                                      {"US", snapshotConfig.US}}
									  .dump();
	}
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

types::units_signed::SignedMeterValue IsaIemDcrController::get_receipt() {
	return helper_get_signed_datatuple("/counter/v1/ocmf/receipt");
}

types::units_signed::SignedMeterValue IsaIemDcrController::get_transaction() {
	return helper_get_signed_datatuple("/counter/v1/ocmf/transaction");
}

void IsaIemDcrController::post_receipt(const std::string& TX) {
    const std::string endpoint = "/counter/v1/ocmf/receipt";
    const std::string payload = nlohmann::ordered_json{{"TX", TX}}
									  .dump();
    auto response = this->http_client->post(endpoint, payload);
    if (response.status_code != 200) {
        throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
    }
}

bool IsaIemDcrController::helper_get_bool_from_OCMFUserIdentificationStatus(types::powermeter::OCMFUserIdentificationStatus IS) {
    return (IS == types::powermeter::OCMFUserIdentificationStatus::ASSIGNED);
}

std::string IsaIemDcrController::helper_get_string_from_OCMFIdentificationLevel(std::optional<types::powermeter::OCMFIdentificationLevel> optIL) {
    std::string result;
    types::powermeter::OCMFIdentificationLevel IL = optIL.value_or(types::powermeter::OCMFIdentificationLevel::UNKNOWN);
    switch(IL) {
		case types::powermeter::OCMFIdentificationLevel::NONE:
			result = "NONE";
			break;
		case types::powermeter::OCMFIdentificationLevel::HEARSAY:
			result = "HEARSAY";
			break;
		case types::powermeter::OCMFIdentificationLevel::TRUSTED:
			result = "TRUSTED";
			break;
		case types::powermeter::OCMFIdentificationLevel::VERIFIED:
			result = "VERIFIED";
			break;
		case types::powermeter::OCMFIdentificationLevel::CERTIFIED:
			result = "CERTIFIED";
			break;
		case types::powermeter::OCMFIdentificationLevel::SECURE:
			result = "SECURE";
			break;
		case types::powermeter::OCMFIdentificationLevel::MISMATCH:
			result = "MISMATCH";
			break;
		case types::powermeter::OCMFIdentificationLevel::INVALID:
			result = "INVALID";
			break;
		case types::powermeter::OCMFIdentificationLevel::OUTDATED:
			result = "OUTDATED";
			break;
		default:
			result = "UNKNOWN";
			break;
	}
    return result;
}

std::string IsaIemDcrController::helper_get_string_from_OCMFIdentificationFlags(types::powermeter::OCMFIdentificationFlags idFlag) {
    std::string result;
    switch(idFlag) {
		case types::powermeter::OCMFIdentificationFlags::RFID_NONE:
			result = "RFID_NONE";
			break;
		case types::powermeter::OCMFIdentificationFlags::RFID_PLAIN:
			result = "RFID_PLAIN";
			break;
		case types::powermeter::OCMFIdentificationFlags::RFID_RELATED:
			result = "RFID_RELATED";
			break;
		case types::powermeter::OCMFIdentificationFlags::RFID_PSK:
			result = "RFID_PSK";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_NONE:
			result = "OCPP_NONE";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_RS:
			result = "OCPP_RS";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH:
			result = "OCPP_AUTH";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_RS_TLS:
			result = "OCPP_RS_TLS";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_AUTH_TLS:
			result = "OCPP_AUTH_TLS";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_CACHE:
			result = "OCPP_CACHE";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_WHITELIST:
			result = "OCPP_WHITELIST";
			break;
		case types::powermeter::OCMFIdentificationFlags::OCPP_CERTIFIED:
			result = "OCPP_CERTIFIED";
			break;
		case types::powermeter::OCMFIdentificationFlags::ISO15118_NONE:
			result = "ISO15118_NONE";
			break;
		case types::powermeter::OCMFIdentificationFlags::ISO15118_PNC:
			result = "ISO15118_PNC";
			break;
		case types::powermeter::OCMFIdentificationFlags::PLMN_NONE:
			result = "PLMN_NONE";
			break;
		case types::powermeter::OCMFIdentificationFlags::PLMN_RING:
			result = "PLMN_RING";
			break;
		case types::powermeter::OCMFIdentificationFlags::PLMN_SMS:
			result = "PLMN_SMS";
			break;
		default:
			result = "UNKNOWN";
			break;
	}
    return result;
}

std::string IsaIemDcrController::helper_get_string_from_OCMFIdentificationType(types::powermeter::OCMFIdentificationType IT) {
    std::string result;
    switch(IT) {
		case types::powermeter::OCMFIdentificationType::DENIED:
			result = "DENIED";
			break;
		case types::powermeter::OCMFIdentificationType::UNDEFINED:
			result = "UNDEFINED";
			break;
		case types::powermeter::OCMFIdentificationType::ISO14443:
			result = "ISO14443";
			break;
		case types::powermeter::OCMFIdentificationType::ISO15693:
			result = "ISO15693";
			break;
		case types::powermeter::OCMFIdentificationType::EMAID:
			result = "EMAID";
			break;
		case types::powermeter::OCMFIdentificationType::EVCCID:
			result = "EVCCID";
			break;
		case types::powermeter::OCMFIdentificationType::EVCOID:
			result = "EVCOID";
			break;
		case types::powermeter::OCMFIdentificationType::ISO7812:
			result = "ISO7812";
			break;
		case types::powermeter::OCMFIdentificationType::CARD_TXN_NR:
			result = "CARD_TXN_NR";
			break;
		case types::powermeter::OCMFIdentificationType::CENTRAL:
			result = "CENTRAL";
			break;
		case types::powermeter::OCMFIdentificationType::CENTRAL_1:
			result = "CENTRAL_1";
			break;
		case types::powermeter::OCMFIdentificationType::CENTRAL_2:
			result = "CENTRAL_2";
			break;
		case types::powermeter::OCMFIdentificationType::LOCAL:
			result = "LOCAL";
			break;
		case types::powermeter::OCMFIdentificationType::LOCAL_1:
			result = "LOCAL_1";
			break;
		case types::powermeter::OCMFIdentificationType::LOCAL_2:
			result = "LOCAL_2";
			break;
		case types::powermeter::OCMFIdentificationType::PHONE_NUMBER:
			result = "PHONE_NUMBER";
			break;
		case types::powermeter::OCMFIdentificationType::KEY_CODE:
			result = "KEY_CODE";
			break;
		default:
			result = "NONE";
			break;
	}
    return result;
}

std::string IsaIemDcrController::helper_get_current_datetime() {
	//Get UTC time
	auto now = std::chrono::system_clock::now();
	//Add configured timezone information
	char signChar = snapshotConfig.timezone[0];
	int offsetHours = std::stoi(snapshotConfig.timezone.substr(1, 2));
    int offsetMinutes = std::stoi(snapshotConfig.timezone.substr(3, 2));
    auto timeOffset = std::chrono::hours(offsetHours) + std::chrono::minutes(offsetMinutes);
    std::time_t nowWithOffset;
	if(signChar == '+') {
		nowWithOffset = std::chrono::system_clock::to_time_t(now + timeOffset);
	} else if(signChar == '-') {
		nowWithOffset = std::chrono::system_clock::to_time_t(now - timeOffset);
	} else {
		throw std::runtime_error("manifest.yaml: Format of timezone not supported. Expected: something like \"+0100\".");
	}
	//Generate and return time in correct format
	std::ostringstream ss;
	ss << std::put_time(gmtime(&nowWithOffset), "%FT%T,000") << snapshotConfig.timezone;
	return ss.str();
}

std::string IsaIemDcrController::helper_remove_first_and_last_char(const std::string& input) {
	if (input.length() <= 1) {
        return ""; 
    }
    return input.substr(1, input.length() - 1);
}

types::units_signed::SignedMeterValue IsaIemDcrController::helper_get_signed_datatuple(std::string endpoint) {
	auto response = this->http_client->get(endpoint);
	types::units_signed::SignedMeterValue retVal;
	if (response.status_code == 200) {
		try {
			retVal.signed_meter_data = response.body;
			retVal.signing_method = "";
			retVal.encoding_method = "OCMF";
			retVal.public_key = get_publickey(true);
			
			return retVal;
		} catch (json::exception& json_error) {
			throw UnexpectedIemDcrResponseBody(
            endpoint, fmt::format("Json error {} for body {}", json_error.what(), response.body));
		}
	} else {
		throw UnexpectedIemDcrResponseCode(endpoint, 200, response);
	}
}

} // namespace module::main
