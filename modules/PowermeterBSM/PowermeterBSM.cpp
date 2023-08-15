// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "PowermeterBSM.hpp"

namespace module {

void PowermeterBSM::init() {

    read_config();
    dump_config_to_log();

    invoke_init(*p_main);
    invoke_init(*p_ac_meter);
}

void PowermeterBSM::ready() {
    invoke_ready(*p_main);
    invoke_ready(*p_ac_meter);
}

//////////////////////////////////////////////////////////////////////
//
// module module implementation helper

void PowermeterBSM::read_config() {

    m_serial_device_name = config.serial_device;
    m_update_interval = std::chrono::seconds(config.update_interval);
    m_watchdog_interval = std::chrono::seconds(config.watchdog_wakeup_interval);
    m_unit_id = config.power_unit_id;
    m_modbus_base_address = protocol_related_types::ModbusRegisterAddress(config.sunspec_base_address);
}

void PowermeterBSM::dump_config_to_log() {
    EVLOG_verbose << __PRETTY_FUNCTION__ << "\n"
                  << " module config     :\n"
                  << " device            : " << m_serial_device_name << "\n"
                  << " update interval   : " << std::to_string(m_update_interval.count()) << "\n"
                  << " watchdog interval : " << std::to_string(m_watchdog_interval.count()) << "\n"
                  << " modbus unit id    : " << std::to_string(m_unit_id) << "\n"
                  << " baud for modbus   : " << std::to_string(config.baud) << "\n"
                  << " use SerialCommHub : " << std::boolalpha << config.use_serial_comm_hub << "\n"
                  << std::endl;
}

everest::connection::SerialDeviceConfiguration::BaudRate baudrate_from_integer(int baudrate) {
    switch (baudrate) {
    case 1200:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_1200;
        break;
    case 2400:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_2400;
        break;
    case 4800:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_4800;
        break;
    case 9600:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_9600;
        break;
    case 19200:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_19200;
        break;
    case 38400:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_38400;
        break;
    case 57600:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_57600;
        break;
    case 115200:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_115200;
        break;
    case 230400:
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_230400;
        break;
    default:
        EVLOG_error << "Error setting up connection to serialport: Undefined baudrate! Defaulting to 9600.\n";
        return everest::connection::SerialDeviceConfiguration::BaudRate::Baud_9600;
        break;
    }
}

transport::AbstractDataTransport::Spt PowermeterBSM::get_data_transport() {

    if (not m_transport_spt) {
        if (config.use_serial_comm_hub) {
            if (r_serial_com_0_connection.empty()) {
                throw std::runtime_error(""s + __PRETTY_FUNCTION__ +
                                         " SerialCommHub enabled, but no connection available.");
            }
            m_transport_spt = std::make_shared<transport::SerialCommHubTransport>((*(r_serial_com_0_connection.at(0))),
                                                                                  m_unit_id, m_modbus_base_address);
        } else {
            auto modbus_transport_spt =
                std::make_shared<transport::ModbusTransport>(m_serial_device_name, m_unit_id, m_modbus_base_address);
            modbus_transport_spt->serial_device().get_serial_device_config().set_baud_rate(
                baudrate_from_integer(config.baud));
            m_transport_spt = modbus_transport_spt;
        }
    }
    return m_transport_spt;
}

} // namespace module
