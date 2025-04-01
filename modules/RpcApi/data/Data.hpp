// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// This contains types for all the JSON objects

namespace data {

class ConnectorInfoStore {};
class HardwareCapabilitiesStore {};
class MeterDataStore {};
class ACChargeParametersStore {};
class DCChargeParametersStore {};
class ACChargeLoopStore {};
class DCChargeLoopStore {};
class DisplayParametersStore {};
class EVSEStatusStore {};
class EVSEInfoStore {};
class ChargerInfoStore {};

class DataStore {
    ConnectorInfoStore connectorinfo;
    HardwareCapabilitiesStore hardwarecapabilities;
    MeterDataStore meterdata;
    ACChargeParametersStore acchargeparameters;
    DCChargeParametersStore dcchargeparameters;
    ACChargeLoopStore acchargeloop;
    DCChargeLoopStore dcchargeloop;
    DisplayParametersStore displayparameters;
    EVSEStatusStore evsestatus;
    EVSEInfoStore evseinfo;
    ChargerInfoStore chargerinfo;
};

}
