// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "powermeterImpl.hpp"
#include "http_client.hpp"
#include <chrono>
#include <string>
#include <thread>

namespace module {
namespace main {

void powermeterImpl::init() {
	EVLOG_info << "Isabellenhuette IEM-DCR: Init started...";
	
	//Check Config values (essential plausibility checks)
    check_config();
	
	// Dependency injection pattern: Create the HTTP client first,
    // then move it into the controller as a constructor argument
    auto http_client =
        std::make_unique<HttpClient>(mod->config.ip_address, mod->config.port_http);
            
    //Create controller object
    this->controller = std::make_unique<IsaIemDcrController>(std::move(http_client), 
			IsaIemDcrController::SnapshotConfig{mod->config.timezone, mod->config.datetime_resync_interval,
            mod->config.resilience_initial_connection_retries, mod->config.resilience_initial_connection_retry_delay,
			mod->config.resilience_transaction_request_retries, mod->config.resilience_transaction_request_retry_delay,
            mod->config.CT, mod->config.CI, mod->config.TT_initial, mod->config.US});
            
    //Store datetime resync interval in a threadsafe manner
    dateTimeResyncInterval.store(mod->config.datetime_resync_interval);
    
    //Check connection with polling REST node gw
    this->controller->call_with_retry([this]() { this->controller->get_gw(); }, 
					mod->config.resilience_initial_connection_retries,
                    mod->config.resilience_initial_connection_retry_delay);
                    
    //Send gw information
    try {
		this->controller->post_gw();
		lastDateTimeSync.store(std::chrono::steady_clock::now());
	} catch (IsaIemDcrController::UnexpectedIemDcrResponseCode& error) {
		EVLOG_warning << "Node /gw seems to be already set. If those values should be updated, please restart IEM-DCR and then also this system.";
		//If gw is already set, not TM information is transfered here. So mark time as invalid for later update in ready function
		lastDateTimeSync.store(std::chrono::steady_clock::now() - std::chrono::hours(48));
	}
	
	//Send initial tariff information
	try {
		if(mod->config.TT_initial.length() > 0) {
			this->controller->post_tariff(mod->config.TT_initial);
		}
	} catch (IsaIemDcrController::UnexpectedIemDcrResponseCode& error) {
		EVLOG_error << "Incorrect config: Value TT_initial could not be set. Please check its value.";
	}
}

void powermeterImpl::ready() {
	// Start the live_measure_publisher thread, which periodically publishes the live measurements of the device
    this->live_measure_publisher_thread = std::thread([this] {
        while (true) {
			//Wait for one second
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            try {         		
				//Publish public key
				this->publish_public_key_ocmf(this->controller->get_publickey(true));
				
				//Publish metervalue node (named powermeter in EVerest) and update status information
				auto meterValueResponse = 		this->controller->get_metervalue();
				types::powermeter::Powermeter 	tmpPowermeter;
				std::string						tmpErrorState;
				bool 							tmpTransactionActive;
				std::tie(tmpPowermeter, tmpErrorState, tmpTransactionActive) = meterValueResponse;
				this->publish_powermeter(tmpPowermeter);
				errorState.store(tmpErrorState);
				transactionActive.store(tmpTransactionActive);
				
				//Debug output :)
				//EVLOG_info << this->controller->get_datetime();
				
				//Update datetime in specified interval
				if(transactionActive.load() == false) {
					const auto now = std::chrono::steady_clock::now();
					const auto elapsed = std::chrono::duration_cast<std::chrono::hours>(now - lastDateTimeSync.load());
					if (elapsed.count() >= dateTimeResyncInterval.load()) {
						this->controller->post_datetime();
						lastDateTimeSync.store(now);
						EVLOG_info << "DateTime resynchronized.";
					}
				}
				
				// Reset previous error (if active)
                if (this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                               "Communication timed out")) {
                    clear_error("powermeter/CommunicationFault", "Communication timed out");
                }
			} catch (HttpClientError& client_error) {
                if (!this->error_state_monitor->is_error_active("powermeter/CommunicationFault",
                                                                "Communication timed out")) {
                    EVLOG_error << "Failed to communicate with the powermeter due to http error: "
                                << client_error.what();
                    auto error =
                        this->error_factory->create_error("powermeter/CommunicationFault", "Communication timed out",
                                                          "This error is raised due to communication timeout");
                    raise_error(error);
                }		
			} catch (const std::exception& e) { 
				EVLOG_error << "Exception in cyclic IEM-DCR communication: " << e.what();
			}
        }
    });
}

types::powermeter::TransactionStartResponse
powermeterImpl::handle_start_transaction(types::powermeter::TransactionReq& value) {
    // your code for cmd start_transaction goes here
    types::powermeter::TransactionStartResponse retVal;
    
    EVLOG_info << "handle_start_transaction() called.";
    
    //Check preconditions
	if(value.evse_id != mod->config.CI && value.evse_id.length() > 0) {
		retVal.status = types::powermeter::TransactionRequestStatus::NOT_SUPPORTED;
		retVal.error = "config: CI does not match evse_id. This is not allowed.";
		EVLOG_error << "Aborted: " << *retVal.error;
		return retVal;
	}
	if(errorState.load() != "0x0000, 0x00000000, 0x00, 0x00") {
		retVal.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
		retVal.error = "IEM-DCR is in error state. XC: " + errorState.load();
		EVLOG_error << "Aborted: " << *retVal.error;
		return retVal;
	}
    
    //Perform action
    try {
		//Stop transaction (if a transaction is still running)
		if(transactionActive) {
			this->controller->call_with_retry([this]() { this->controller->post_receipt("E"); },
										mod->config.resilience_transaction_request_retries,
										mod->config.resilience_transaction_request_retry_delay);
		}	
		//Create user
		if((static_cast<std::string>(value.identification_data.value_or(""))).length() <= 0) {
			this->controller->call_with_retry([this, value]() { this->controller->post_user(
										value.identification_status, value.identification_level,
										value.identification_flags, value.identification_type,
										value.transaction_id, value.tariff_text); }, 
							mod->config.resilience_transaction_request_retries,
							mod->config.resilience_transaction_request_retry_delay);
		} else {
			this->controller->call_with_retry([this, value]() { this->controller->post_user(
										value.identification_status, value.identification_level,
										value.identification_flags, value.identification_type,
										value.identification_data, value.tariff_text); }, 
							mod->config.resilience_transaction_request_retries,
							mod->config.resilience_transaction_request_retry_delay);
		}
		//Start transaction
		this->controller->call_with_retry([this]() { this->controller->post_receipt("B"); },
									mod->config.resilience_transaction_request_retries,
									mod->config.resilience_transaction_request_retry_delay);
		//Prepare positive response							
		retVal.status = types::powermeter::TransactionRequestStatus::OK;
		retVal.error = "";
	} catch (const std::exception& e) { 
		retVal.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
		retVal.error = e.what();
		EVLOG_error << "Aborted: " << retVal.error.value_or("");
	}     
    
    return retVal;
}

types::powermeter::TransactionStopResponse powermeterImpl::handle_stop_transaction(std::string& transaction_id) {
    // your code for cmd stop_transaction goes here
    types::powermeter::TransactionStopResponse retVal;
    
    EVLOG_info << "handle_stop_transaction() called.";
    
    if(transactionActive) {
		try {
			//Stop transaction
			this->controller->call_with_retry([this]() { this->controller->post_receipt("E"); },
										mod->config.resilience_transaction_request_retries,
										mod->config.resilience_transaction_request_retry_delay);
			//Wait for signature calculation
			std::this_thread::sleep_for(std::chrono::milliseconds(200));	
			//Read receipt
			retVal.signed_meter_value = this->controller->call_with_retry([this]() { return this->controller->get_receipt(); },
										mod->config.resilience_transaction_request_retries,
										mod->config.resilience_transaction_request_retry_delay);
			//Prepare positive response	
			retVal.status = types::powermeter::TransactionRequestStatus::OK;
			retVal.error = "";
		} catch (const std::exception& e) { 
			retVal.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
			retVal.error = e.what();
			EVLOG_error << "Aborted: " << retVal.error.value_or("");
		}     
	} else {
		//No transaction running. So return last transaction (if available)
		try {
			retVal.signed_meter_value = this->controller->get_transaction();
			retVal.status = types::powermeter::TransactionRequestStatus::OK;
			retVal.error = "";
		} catch (std::exception& e) {
			retVal.status = types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR;
			retVal.error  = std::string(e.what()) + " Maybe no transaction to return?";
			EVLOG_warning << "Aborted: " << retVal.error.value_or("");
		}
	}
    
    return retVal;
}

void powermeterImpl::check_config() {
	if(mod->config.ip_address.length() <= 0) {
		EVLOG_error << "Incorrect module config: parameter ip_address is empty." << std::endl;
		throw std::runtime_error("ip_address invalid. Please check configuration.");
	}
	if(mod->config.port_http < 0) {
		EVLOG_error << "Incorrect module config: parameter port_http has a negative value." << std::endl;
		throw std::runtime_error("port_http invalid. Please check configuration.");
	}
    if(mod->config.timezone.length() != 5) {
		EVLOG_error << "Incorrect module config: parameter timezone has invalid length. 5 characters expected, e.g. +0200" << std::endl;
		throw std::runtime_error("Timezone invalid. Please check configuration.");
	}
	if(mod->config.timezone[0] != '+' && mod->config.timezone[0] != '-') {
		EVLOG_error << "Incorrect module config: parameter timezone has invalid format. It must begin with + or - char." << std::endl;
		throw std::runtime_error("Timezone invalid. Please check configuration.");
	}
	if(mod->config.datetime_resync_interval <= 0) {
		EVLOG_error << "Incorrect module config: The value of parameter datetime_resync_interval must be 1 or greater." << std::endl;
		throw std::runtime_error("datetime_resync_interval invalid. Please check configuration.");
	}
	if(mod->config.resilience_initial_connection_retries < 0) {
		EVLOG_error << "Incorrect module config: parameter resilience_initial_connection_retries has a negative value." << std::endl;
		throw std::runtime_error("port_http resilience_initial_connection_retries. Please check configuration.");
	}
	if(mod->config.resilience_initial_connection_retry_delay < 0) {
		EVLOG_error << "Incorrect module config: parameter resilience_initial_connection_retry_delay has a negative value." << std::endl;
		throw std::runtime_error("port_http resilience_initial_connection_retry_delay. Please check configuration.");
	}
	if(mod->config.resilience_transaction_request_retries < 0) {
		EVLOG_error << "Incorrect module config: parameter resilience_transaction_request_retries has a negative value." << std::endl;
		throw std::runtime_error("port_http resilience_transaction_request_retries. Please check configuration.");
	}
	if(mod->config.resilience_transaction_request_retry_delay < 0) {
		EVLOG_error << "Incorrect module config: parameter resilience_transaction_request_retry_delay has a negative value." << std::endl;
		throw std::runtime_error("port_http resilience_transaction_request_retry_delay. Please check configuration.");
	}
	if(mod->config.CT.length() <= 0) {
		EVLOG_error << "Incorrect module config: parameter CT is empty." << std::endl;
		throw std::runtime_error("CT invalid. Please check configuration.");
	}
	if(mod->config.CI.length() <= 0) {
		EVLOG_error << "Incorrect module config: parameter CI is empty." << std::endl;
		throw std::runtime_error("CI invalid. Please check configuration.");
	}
}

} // namespace main
} // namespace module
