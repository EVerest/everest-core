//! Module for Iskra's WM3M4 & WM3M4C three-phase electrical energy meters.
//!
//! The implementation follows the user manual
//! <https://www.iskra.eu/f/docs/Smart-energy-meters/K_WM3M4_EN_22433922_Users_manual_Ver_1.14.pdf>
//!
//! ## Example usage:
//!
//! To try out the Iskra meter you can use the following dummy config below.
//!
//! ```yaml
//!active_modules:
//!  power_meter_1:
//!    module: RsIskraMeter
//!    config_module:
//!      powermeter_device_id: 33
//!    connections:
//!      modbus:
//!      - module_id: serial_comm_hub_1
//!        implementation_id: main
//!  serial_comm_hub_1:
//!    module: SerialCommHub
//!    config_implementation:
//!      main:
//!        serial_port: /dev/ttyUSB0
//!        baudrate: 115200
//!        initial_timeout_ms: 10000
//!        within_message_timeout_ms: 100
//!        max_packet_size: 100
//! ```
//!
//! You can start and stop transactions by publishing following messages to
//! mqtt
//!
//! ```sh
//! mosquitto_pub -t everest/power_meter_1/meter/cmd -m '{"data":{"args":{"value":{"cable_id":1,"client_id":"","evse_id":"DEABCD312ABC11","tariff_id":1,"transaction_id":"foobarbaz","user_data":""}},"id":"foo","origin":"bar"},"name":"start_transaction","type":"call"}'
//! mosquitto_pub -t everest/power_meter_1/meter/cmd -m '{"data":{"args":{"transaction_id":"foobarbaz"}, "id":"foo","origin":"bar"},"name":"stop_transaction","type":"call"}'
//! ```
#![allow(non_snake_case, non_camel_case_types)]

include!(concat!(env!("OUT_DIR"), "/generated.rs"));
mod utils;

use anyhow::Result;
use chrono::{Local, Offset, Utc};
use everestrs::serde as everest_serde;
use everestrs::serde_json as everest_serde_json;
use generated::types::powermeter::{
    Powermeter, TransactionRequestStatus, TransactionStartResponse, TransactionStopResponse,
};
use generated::types::serial_comm_hub_requests::{StatusCodeEnum, VectorUint16};
use generated::types::units::{Current, Energy, Frequency, Power, ReactivePower, Voltage};
use generated::types::units_signed::SignedMeterValue;
use generated::{get_config, Module, ModuleConfig, SerialCommunicationHubClientPublisher};
use std::fmt::Debug;
use std::sync::{Arc, Mutex};
use std::time::Duration;
use utils::{
    counter, create_ocmf, create_random_meter_session_id, from_t5_format, from_t6_format,
    string_to_vec, to_8_string, to_hex_string,
};

/// Public key prefix for transparency software, defined under 6.5.14.
const PUBLIC_KEY_PREFIX: &str = "3059301306072A8648CE3D020106082A8648CE3D03010703420004";

/// The charging state from register 7000, defined at table 6.
#[derive(PartialEq, Debug)]
enum IskraMaterState {
    Idle,
    Active,
    Active_after_power_failure,
    Active_after_reset,
    Unknown,
}

impl IskraMaterState {
    fn from_register(val: u16) -> Self {
        match val {
            0 => IskraMaterState::Idle,
            1 => IskraMaterState::Active,
            2 => IskraMaterState::Active_after_power_failure,
            3 => IskraMaterState::Active_after_reset,
            _ => IskraMaterState::Unknown,
        }
    }
}

#[derive(PartialEq, Debug)]
/// The signature status values defined at table 11.
enum SignatureStatus {
    NotInitialized,
    Idle,
    SignatureInProgress,
    SignatureOK,
    InvalidDateTime,
    CheckSumError,
    InvalidCommand,
    InvalidState,
    InvalidMeasurement,
    TestModeError,
    VerifyStateError,
    SignatureStateError,
    KeyPairGenerationError,
    SHAFailed,
    InitFailed,
    DataNotLocked,
    ConfigNotLocked,
    VerifyError,
    PublicKeyError,
    InvalidMessageFormat,
    InvalidMessageSize,
    SignatureError,
    UndefinedError,
}

impl TryFrom<u16> for SignatureStatus {
    type Error = anyhow::Error;
    fn try_from(value: u16) -> std::result::Result<Self, Self::Error> {
        match value {
            0 => Ok(SignatureStatus::NotInitialized),
            1 => Ok(SignatureStatus::Idle),
            2 => Ok(SignatureStatus::SignatureInProgress),
            15 => Ok(SignatureStatus::SignatureOK),
            128 => Ok(SignatureStatus::InvalidDateTime),
            129 => Ok(SignatureStatus::CheckSumError),
            130 => Ok(SignatureStatus::InvalidCommand),
            131 => Ok(SignatureStatus::InvalidState),
            132 => Ok(SignatureStatus::InvalidMeasurement),
            133 => Ok(SignatureStatus::TestModeError),
            243 => Ok(SignatureStatus::VerifyStateError),
            244 => Ok(SignatureStatus::SignatureStateError),
            245 => Ok(SignatureStatus::KeyPairGenerationError),
            246 => Ok(SignatureStatus::SHAFailed),
            247 => Ok(SignatureStatus::InitFailed),
            248 => Ok(SignatureStatus::DataNotLocked),
            249 => Ok(SignatureStatus::ConfigNotLocked),
            250 => Ok(SignatureStatus::VerifyError),
            251 => Ok(SignatureStatus::PublicKeyError),
            252 => Ok(SignatureStatus::InvalidMessageFormat),
            253 => Ok(SignatureStatus::InvalidMessageSize),
            254 => Ok(SignatureStatus::SignatureError),
            255 => Ok(SignatureStatus::UndefinedError),
            unknown => Err(anyhow::anyhow!("Failed to convert the value {unknown}")),
        }
    }
}

/// Converts the EVerest type to our internal.
impl From<StatusCodeEnum> for Result<()> {
    fn from(value: StatusCodeEnum) -> Self {
        match value {
            StatusCodeEnum::Success => Ok(()),
            StatusCodeEnum::Error => anyhow::bail!("StatusCodeEnum::Error"),
        }
    }
}

/// Custom extension of the auto generated type.
impl generated::types::serial_comm_hub_requests::Result {
    /// We have to check if the received data matches the expected size.
    /// * `size`: The expected size of the vector.
    fn into_vec(self, size: usize) -> Result<Vec<u16>> {
        match self.status_code {
            StatusCodeEnum::Success => match self.value {
                None => anyhow::bail!("Received None as value"),
                Some(value) => {
                    if value.len() != size {
                        anyhow::bail!("Expected size {}, received size {size}", value.len())
                    }
                    Ok(value.into_iter().map(|v| v as u16).collect())
                }
            },
            StatusCodeEnum::Error => anyhow::bail!("StatusCodeEnum::Error"),
        }
    }
}

/// Custom conversion to `Result<[u16; N]>`
///
/// Similar to `generated::types::serial_comm_hub_requests::Result::into_vec`
/// but returns an array.
impl<const N: usize> From<generated::types::serial_comm_hub_requests::Result> for Result<[u16; N]> {
    fn from(value: generated::types::serial_comm_hub_requests::Result) -> Self {
        match value.status_code {
            StatusCodeEnum::Success => match value.value {
                None => anyhow::bail!("Received None as value"),
                Some(inner) => {
                    if inner.len() != N {
                        anyhow::bail!("Expected size {}, received size {N}", inner.len())
                    }
                    let mut res = [0; N];
                    for (ss, dd) in std::iter::zip(inner, &mut res) {
                        *dd = ss as u16;
                    }
                    Ok(res)
                }
            },
            StatusCodeEnum::Error => anyhow::bail!("StatusCodeEnum::Error"),
        }
    }
}

/// Custom extension of the auto generated `TransactionStartResponse`.
impl TransactionStartResponse {
    /// Constructs an error response from the input.
    /// * `error`: The error type.
    fn from_err<E>(error: &E) -> Self
    where
        E: Debug,
    {
        Self {
            error: Some(format!("{error:?}")),
            status: TransactionRequestStatus::UNEXPECTED_ERROR,
            transaction_max_stop_time: None,
            transaction_min_stop_time: None,
        }
    }
}

/// Custom extension of the auto generated `TransactionStopResponse`.
impl TransactionStopResponse {
    /// Constructs an error response from the input.
    /// * `error`: The error type.
    fn from_err<E>(error: &E) -> Self
    where
        E: Debug,
    {
        Self {
            error: Some(format!("{error:?}")),
            start_signed_meter_value: None,
            signed_meter_value: None,
            status: TransactionRequestStatus::UNEXPECTED_ERROR,
        }
    }
}

#[derive(everest_serde::Serialize, Clone)]
#[serde(crate = "everest_serde")]
/// The serialization according to the Open charge metering format. See
/// https://github.com/SAFE-eV/OCMF-Open-Charge-Metering-Format/blob/master/OCMF-en.md
/// for details.
///
/// The struct does not implement everything and also our config does not
/// implement everything. The allowed fields (and which can be set by the user)
/// are defined under the Iskra's manual chapter 6.5.15.
struct OcmfData {
    #[serde(rename = "FV")]
    /// Version of the data format in the representation.
    format_version: String,

    #[serde(rename = "GI")]
    /// Identifier of the manufacturer for the system which has generated the
    /// present data
    gateway_identification: String,

    #[serde(rename = "GS")]
    /// Serial number of the above mentioned system.
    gateway_serial: String,

    #[serde(rename = "GV")]
    /// Version designation of the manufacturer for the software.
    gateway_version: String,

    #[serde(rename = "PG")]
    /// Pagination of the entire data set, i.e. the data that is combined in one
    /// signature.
    pagination: String,

    #[serde(rename = "MV")]
    /// Manufacturer identification of the meter, name of the manufacturer.
    meter_vendor: String,

    #[serde(rename = "MM")]
    /// Model identification of the meter.
    meter_model: String,

    #[serde(rename = "MS")]
    /// Serial number of the meter.
    meter_serial: String,

    #[serde(rename = "MF")]
    /// Firmware version of the meter.
    meter_firmware: String,

    #[serde(rename = "IS")]
    /// General status for user assignment.
    identification_status: bool,

    #[serde(rename = "IF")]
    /// Detailed statements about the user assignment.
    identification_flags: Vec<String>,

    #[serde(rename = "IT")]
    /// Type of identification data.
    identification_type: String,

    #[serde(rename = "ID")]
    /// The actual identification data according to the type.
    identification_data: String,

    #[serde(rename = "CT")]
    /// Type of the specification for the identification of the charge point.
    charge_point_identification_type: String,

    #[serde(rename = "CI")]
    /// Identification information for the charge point.
    charge_point_identification: String,

    #[serde(rename = "RD")]
    /// Additional readings.
    readings: Vec<String>,
}

impl Default for OcmfData {
    fn default() -> Self {
        Self {
            format_version: "1.0".to_string(),
            gateway_identification: String::default(),
            gateway_serial: String::default(),
            gateway_version: String::default(),
            pagination: String::default(),
            meter_vendor: String::default(),
            meter_model: String::default(),
            meter_serial: String::default(),
            meter_firmware: String::default(),
            identification_status: true,
            // See table 13 for details.
            identification_flags: vec!["RFID_PLAIN".to_string()],
            // See table 17 for details.
            identification_type: "NONE".to_string(),
            identification_data: String::default(),
            // See table 18 for details.
            charge_point_identification_type: "EVSEID".to_string(),
            charge_point_identification: String::default(),
            readings: Vec::default(),
        }
    }
}

/// Conversion from the EVerest config to the OcmfData.
impl From<&ModuleConfig> for OcmfData {
    fn from(value: &ModuleConfig) -> Self {
        // Below we have to replace the `|` with something (here whitespace),
        // since otherwise the transpacency tool will not accept the signed
        // data. See https://safe-ev.org/de/transparenzsoftware/e-mobilist/.
        let sanitize = |user_string: &str| {
            if user_string.contains("|") {
                log::warn!("Removing the forbidden symbol`|` from {user_string}");
            }
            user_string.replace("|", " ")
        };

        OcmfData {
            format_version: sanitize(&value.ocmf_format_version),
            gateway_identification: sanitize(&value.ocmf_gateway_identification),
            gateway_serial: sanitize(&value.ocmf_gateway_serial),
            gateway_version: sanitize(&value.ocmf_gateway_version),
            charge_point_identification_type: sanitize(
                &value.ocmf_charge_point_identification_type,
            ),
            charge_point_identification: sanitize(&value.ocmf_charge_point_identification),
            ..Default::default()
        }
    }
}

/// Toy retry module
mod retry {

    /// The state the functions have to return.
    pub enum RetryState {
        /// If you return this, we keep trying until the timeout.
        KeepTrying,

        /// If you return this, we're done.
        Done,
    }

    type RetryResult = anyhow::Result<RetryState>;

    pub fn retry<F>(func: F, duration: std::time::Duration) -> anyhow::Result<()>
    where
        F: Fn() -> RetryResult,
    {
        let deadline = std::time::SystemTime::now() + duration;
        let sleep = std::cmp::min(duration, std::time::Duration::from_millis(10));
        while std::time::SystemTime::now() < deadline {
            match func()? {
                RetryState::Done => return Ok(()),
                RetryState::KeepTrying => {
                    std::thread::sleep(sleep);
                }
            }
        }
        anyhow::bail!("Retry failed - timeout after {duration:?}");
    }
}

// Below we're using the type state pattern to implement a small state machine
// with the states `InitState` and `ReadyState`.

/// Initial state. We're moving from this state to `ReadyState` once we're fully
/// Initialized.
#[derive(Clone)]
struct InitState {
    /// Modbus id of the device.
    device_id: i64,

    /// The base ocmf data.
    ocmf_data: OcmfData,
}

impl InitState {
    fn new(device_id: i64, ocmf_data: OcmfData) -> Self {
        Self {
            device_id,
            ocmf_data,
        }
    }
}

/// State where we're ready for publishing.
#[derive(Clone)]
struct ReadyState {
    /// Modbus id of the device.
    device_id: i64,

    /// The base ocmf data.
    ocmf_data: OcmfData,

    /// The interface to the serial comm module.
    serial_comm_pub: SerialCommunicationHubClientPublisher,

    /// Public key
    public_key: Arc<Mutex<Option<String>>>,
}

impl ReadyState {
    /// The `ReadyState` can only be constructed from `InitState`.
    fn new(init_state: InitState, serial_comm_pub: SerialCommunicationHubClientPublisher) -> Self {
        Self {
            device_id: init_state.device_id,
            ocmf_data: init_state.ocmf_data,
            serial_comm_pub,
            public_key: Arc::new(Mutex::new(None)),
        }
    }

    /// Reads `N` registers, starting at `first_register_address`.
    fn read_input_registers_fixed<const N: usize>(
        &self,
        first_register_address: u16,
    ) -> Result<[u16; N]> {
        self.serial_comm_pub
            .modbus_read_input_registers(first_register_address as i64, N as i64, self.device_id)?
            .into()
    }

    fn read_holding_registers(
        &self,
        first_register_address: u16,
        num_registers_to_read: u16,
    ) -> Result<Vec<u16>> {
        self.serial_comm_pub
            .modbus_read_holding_registers(
                first_register_address as i64,
                num_registers_to_read as i64,
                self.device_id,
            )?
            .into_vec(num_registers_to_read as usize)
    }

    /// Same as `read_holding_registers` but returns a compile-time known slice.
    /// Use indices to access the members and the compiler will check for out
    /// of bounds.
    fn read_holding_registers_fixed<const N: usize>(
        &self,
        first_register_address: u16,
    ) -> Result<[u16; N]> {
        self.serial_comm_pub
            .modbus_read_holding_registers(first_register_address as i64, N as i64, self.device_id)?
            .into()
    }

    fn write_multiple_registers(&self, first_register_address: u16, data: &[u16]) -> Result<()> {
        self.serial_comm_pub
            .modbus_write_multiple_registers(
                VectorUint16 {
                    data: data.iter().copied().map(|v| v as i64).collect(),
                },
                first_register_address as i64,
                self.device_id,
            )?
            .into()
    }

    fn write_single_register(&self, register_address: u16, data: u16) -> Result<()> {
        self.serial_comm_pub
            .modbus_write_single_register(data as i64, register_address as i64, self.device_id)?
            .into()
    }

    fn read_state(&self) -> Result<IskraMaterState> {
        let var = self.read_holding_registers_fixed::<1>(7000)?;
        let state = IskraMaterState::from_register(var[0]);
        log::info!("State register {:?} mapped to state {state:?}", var[0]);
        Ok(state)
    }

    fn read_command_status(&self) -> Result<SignatureStatus> {
        let var = self.read_holding_registers_fixed::<1>(7052)?;
        let state = var[0].try_into()?;
        log::info!(
            "Command status register {:?} mapped to state {state:?}",
            var[0]
        );
        Ok(state)
    }

    fn set_time(&self) -> Result<()> {
        let ts = Utc::now().timestamp();
        let offset_min = Local::now().offset().fix().local_minus_utc() / 60;
        // Write time
        log::info!(
            "Writing time {:X}, {:X} {:X} with offset {offset_min}",
            ts,
            (ts >> 16 & 0xFFFF),
            (ts & 0xFFFF)
        );
        self.write_single_register(7054, (ts >> 16 & 0xFFFF) as u16)?;
        self.write_single_register(7055, (ts & 0xFFFF) as u16)?;
        // Write timezone
        self.write_single_register(7053, offset_min as u16)?;
        // Set time as synchronized
        self.write_single_register(7071, 2)?;
        Ok(())
    }

    fn read_device_group(&self) -> Result<String> {
        let registers = self.read_input_registers_fixed::<1>(1)?;
        to_8_string(&registers)
    }

    fn read_device_model(&self) -> Result<String> {
        let registers = self.read_input_registers_fixed::<8>(1)?;
        to_8_string(&registers)
    }

    fn read_device_serial(&self) -> Result<String> {
        let registers = self.read_input_registers_fixed::<4>(9)?;
        to_8_string(&registers)
    }

    fn read_t6(&self, first_register_address: u16) -> Result<f64> {
        let var = self.read_input_registers_fixed::<2>(first_register_address)?;
        Ok(from_t6_format(var))
    }

    fn read_t5(&self, first_register_address: u16) -> Result<f64> {
        let var = self.read_input_registers_fixed::<2>(first_register_address)?;
        Ok(from_t5_format(var))
    }

    fn read_counter(
        &self,
        exp_register_address: u16,
        counter_register_address: u16,
    ) -> Result<f64> {
        let exp_register = self.read_input_registers_fixed::<1>(exp_register_address)?;
        // Signed Value (16 bits)
        let var = self.read_input_registers_fixed::<2>(counter_register_address)?;
        Ok(counter(var, exp_register[0]))
    }

    fn read_meter_value(&self) -> Result<Powermeter> {
        let resp = Powermeter {
            var: Some(ReactivePower {
                l_1: Some(self.read_t6(150)?),
                l_2: Some(self.read_t6(152)?),
                l_3: Some(self.read_t6(154)?),
                total: self.read_t6(148)?,
            }),
            current_a: Some(Current {
                dc: Option::None,
                l_1: Some(self.read_t6(126)?),
                l_2: Some(self.read_t6(128)?),
                l_3: Some(self.read_t6(130)?),
                n: Option::None,
            }),
            energy_wh_export: Some(Energy {
                l_1: Option::None,
                l_2: Option::None,
                l_3: Option::None,
                total: self.read_counter(415, 420)?,
            }),
            energy_wh_import: Energy {
                l_1: Option::None,
                l_2: Option::None,
                l_3: Option::None,
                total: self.read_counter(414, 418)?,
            },
            // TODO(kch) why freq values are different?
            frequency_hz: Some(Frequency {
                l_1: self.read_t5(105)?,
                l_2: Option::None,
                l_3: Option::None,
            }),
            // TODO(kch) meter_id
            meter_id: Option::None,
            phase_seq_error: Option::None,
            power_w: Some(Power {
                l_1: Some(self.read_t6(142)?),
                l_2: Some(self.read_t6(144)?),
                l_3: Some(self.read_t6(146)?),
                total: self.read_t6(140)?,
            }),
            timestamp: Utc::now().to_rfc3339(),
            voltage_v: Some(Voltage {
                dc: Option::None,
                l_1: Some(self.read_t5(107)?),
                l_2: Some(self.read_t5(109)?),
                l_3: Some(self.read_t5(111)?),
            }),
            current_a_signed: None,
            energy_wh_export_signed: None,
            energy_wh_import_signed: None,
            frequency_hz_signed: None,
            power_w_signed: None,
            signed_meter_value: None,
            var_signed: None,
            voltage_v_signed: None,
        };
        Ok(resp)
    }

    fn write_metadata(&self, evse_id: &str) -> Result<()> {
        let mut ocmf_data = self.ocmf_data.clone();
        ocmf_data.identification_data = format!("{} {}", evse_id, create_random_meter_session_id());

        let message = everest_serde_json::to_string(&ocmf_data)?;
        let data = string_to_vec(&message);
        self.write_multiple_registers(7100, &data)?;
        // write bytes
        log::info!("Writing length {:?}", message.len());
        self.write_single_register(7056, message.len() as u16)?;
        let resp = self.read_holding_registers(7100, data.len() as u16)?;
        let mut resp_str = to_8_string(&resp)?;
        resp_str.truncate(message.len());
        log::info!("Initial string: {resp_str}");
        Ok(())
    }

    fn read_signed_meter_values(&self) -> Result<String> {
        let length_of_values = self.read_holding_registers_fixed::<1>(7057)?[0];
        log::info!("Length of values: {}", length_of_values);
        let registers_amount = (length_of_values + 1) / 2;
        let regs = self.read_holding_registers(7612, registers_amount)?;
        let mut json_value = to_8_string(&regs)?;
        json_value.truncate(length_of_values as usize);
        log::info!("Read the signed meter values: {}", json_value);
        Ok(json_value)
    }

    fn read_signature(&self) -> Result<String> {
        let length_of_signature = self.read_holding_registers_fixed::<1>(7058)?[0];
        log::info!("Length of signature: {}", length_of_signature);
        let registers_amount = (length_of_signature + 1) / 2;
        let regs = self.read_holding_registers(8188, registers_amount)?;
        let mut signature = to_hex_string(regs);
        signature.truncate((length_of_signature * 2) as usize);
        log::info!("Read the signature: {}", signature);
        Ok(signature)
    }

    /// For 15 seconds and checks the signature status every 10 ms
    fn check_signature_status(&self) -> Result<()> {
        retry::retry(
            || {
                let status = self.read_command_status()?;
                match status {
                    SignatureStatus::NotInitialized
                    | SignatureStatus::Idle
                    | SignatureStatus::SignatureInProgress => Ok(retry::RetryState::KeepTrying),
                    SignatureStatus::SignatureOK => Ok(retry::RetryState::Done),
                    error => anyhow::bail!("Error state {error:?}"),
                }
            },
            Duration::from_secs(15),
        )
    }

    fn start_transaction(
        &self,
        req: generated::types::powermeter::TransactionReq,
    ) -> Result<TransactionStartResponse> {
        // We can start transaction only in Idle state
        let state = self.read_state()?;
        match state {
            IskraMaterState::Active | IskraMaterState::Active_after_reset => {
                log::error!("Unexpected state {state:?}, trying to stop active transaction");
                self.stop_transaction()?;
            }
            IskraMaterState::Active_after_power_failure => {
                // For now, we just cancel any active transaction after a power failure,
                // but in the future, we might want to handle this differently.
                log::error!("Unexpected state {state:?}, trying to stop stuck transaction");
                self.set_time()?;
                self.write_metadata(&req.evse_id)?;
                self.stop_transaction()?;
                log::info!("Stopped stuck transaction");
            }

            IskraMaterState::Idle => {}
            IskraMaterState::Unknown => {
                log::warn!("Unknown state");
            }
        }
        self.set_time()?;
        log::info!("Set time");
        // set algorithm
        self.write_single_register(7059, 0)?;
        self.write_metadata(&req.evse_id)?;

        // Finally send start transaction, we are sending 'B'
        self.write_single_register(7051, 0x4200)?;
        log::info!("Started transaction.");

        // Wait until the meter is active.
        retry::retry(
            || {
                let state = self.read_state()?;
                match state {
                    IskraMaterState::Active => Ok(retry::RetryState::Done),
                    _ => Ok(retry::RetryState::KeepTrying),
                }
            },
            std::time::Duration::from_secs(1),
        )?;

        self.check_signature_status()?;
        Ok(TransactionStartResponse {
            error: Option::None,
            transaction_min_stop_time: Option::None,
            status: TransactionRequestStatus::OK,
            transaction_max_stop_time: Option::None,
        })
    }

    fn stop_transaction(&self) -> Result<generated::types::powermeter::TransactionStopResponse> {
        // We can start transaction only in Idle state
        let state = self.read_state()?;
        match state {
            IskraMaterState::Idle => {
                log::error!("The state of meter is Idle and we can not stop transaction",);
                anyhow::bail!("Transaction not started");
            }
            IskraMaterState::Active
            | IskraMaterState::Active_after_power_failure
            | IskraMaterState::Active_after_reset => {}
            IskraMaterState::Unknown => {
                log::warn!("Unknown state")
            }
        }

        // The state for start transaction is incorrect
        self.write_single_register(7051, 0x7200)?;

        // Wait until the meter is idle.
        retry::retry(
            || {
                let state = self.read_state()?;
                match state {
                    IskraMaterState::Idle => Ok(retry::RetryState::Done),
                    _ => Ok(retry::RetryState::KeepTrying),
                }
            },
            std::time::Duration::from_secs(1),
        )?;

        self.check_signature_status()?;
        let signature = self.read_signature()?;
        let signed_meter_values = self.read_signed_meter_values()?;
        Ok(TransactionStopResponse {
            error: Option::None,
            // Iskra meter has both start and stop snapshot in one
            // OCMF dataset. So we don't need to send the start snapshot.
            start_signed_meter_value: None,
            signed_meter_value: Some(SignedMeterValue {
                signed_meter_data: create_ocmf(signed_meter_values, signature),
                signing_method: String::new(),
                encoding_method: "OCMF".to_string(),
                public_key: self.read_public_key().ok(),
                timestamp: None,
            }),
            status: TransactionRequestStatus::OK,
        })
    }

    /// Read the public key once and cache it.
    fn read_public_key(&self) -> Result<String> {
        let mut lock = self.public_key.lock().expect("Never poisoned");
        match &*lock {
            Some(key) => Ok(key.clone()),
            None => {
                let regs = self.read_holding_registers(8124, 32)?;

                let key = format!("{}{}", PUBLIC_KEY_PREFIX, &to_hex_string(regs));
                *lock = Some(key.clone());
                Ok(key)
            }
        }
    }
}

/// The state machine of this module.
enum StateMachine {
    InitState(InitState),
    ReadyState(ReadyState),
}

/// Main class implementing all EVerest traits.
struct IskraMeter {
    state_machine: Mutex<StateMachine>,
}

impl generated::OnReadySubscriber for IskraMeter {
    fn on_ready(&self, publishers: &generated::ModulePublisher) {
        let mut lock = self.state_machine.lock().unwrap();
        let StateMachine::InitState(ref init_state) = *lock else {
            log::warn!("StateMachine already initialized");
            return;
        };

        // Update from `InitState` to `ReadyState`.
        let ready_state = ReadyState::new(init_state.clone(), publishers.modbus.clone());

        fn print_spec<R, E>(res: &Result<R, E>, name: &str)
        where
            R: Debug,
            E: Debug,
        {
            match res {
                Ok(ok) => log::info!("{name}: {ok:?}"),
                Err(err) => log::error!("Failed to read {name}: {err:?}"),
            };
        }
        print_spec(&ready_state.read_device_group(), "device group");
        print_spec(&ready_state.read_device_model(), "device model");
        print_spec(&ready_state.read_device_serial(), "device serial");

        let ready_state_clone = ready_state.clone();
        let power_meter_clone = publishers.meter.clone();
        std::thread::spawn(move || loop {
            std::thread::sleep(std::time::Duration::from_secs(5));
            let res = ready_state_clone.read_meter_value();
            match res {
                Ok(meter) => {
                    log::info!("Got meter value {:?}", meter);
                    match power_meter_clone.powermeter(meter) {
                        Ok(_) => log::info!("Successfully published meter value"),
                        Err(e) => log::error!("Failed to post meter values {:?}", e),
                    }
                }
                Err(e) => log::error!("Failed to read meter value {:?}", e),
            }
        });

        // Finally update the state in the lock.
        *lock = StateMachine::ReadyState(ready_state);
    }
}

impl generated::SerialCommunicationHubClientSubscriber for IskraMeter {}

impl generated::PowermeterServiceSubscriber for IskraMeter {
    fn start_transaction(
        &self,
        _context: &generated::Context,
        value: generated::types::powermeter::TransactionReq,
    ) -> everestrs::Result<generated::types::powermeter::TransactionStartResponse> {
        let lock = self
            .state_machine
            .lock()
            .map_err(|_| ::everestrs::Error::InvalidArgument("Internal error"))?;

        let StateMachine::ReadyState(ready_state) = &*lock else {
            return Err(::everestrs::Error::InvalidArgument("Not initialized"));
        };

        let res = ready_state.start_transaction(value);

        match res {
            Ok(result) => Ok(result),
            Err(e) => {
                log::error!("Failed to start transaction {:?}", e);
                Ok(TransactionStartResponse::from_err(&e))
            }
        }
    }

    fn stop_transaction(
        &self,
        _context: &generated::Context,
        _transaction_id: String,
    ) -> everestrs::Result<generated::types::powermeter::TransactionStopResponse> {
        let lock = self
            .state_machine
            .lock()
            .map_err(|_| ::everestrs::Error::InvalidArgument("Internal error"))?;

        let StateMachine::ReadyState(ready_state) = &*lock else {
            return Err(::everestrs::Error::InvalidArgument("Not initialized"));
        };

        let res = ready_state.stop_transaction();

        match res {
            Ok(result) => Ok(result),
            Err(e) => {
                log::error!("Failed to stop transaction {:?}", e);
                Ok(TransactionStopResponse::from_err(&e))
            }
        }
    }
}

fn main() {
    let config = get_config();
    let class = Arc::new(IskraMeter {
        state_machine: Mutex::new(StateMachine::InitState(InitState::new(
            config.powermeter_device_id,
            (&config).into(),
        ))),
    });

    let _module = Module::new(class.clone(), class.clone(), class.clone());

    loop {
        std::thread::sleep(std::time::Duration::from_secs(1));
    }
}

#[cfg(test)]
mod tests {

    use self::generated::types::powermeter::TransactionReq;

    use super::*;
    use mockall::predicate::eq;

    /// Helper to produce the class  under test.
    fn make_ready_state(publisher: SerialCommunicationHubClientPublisher) -> ReadyState {
        ReadyState::new(InitState::new(1234, OcmfData::default()), publisher)
    }

    #[test]
    fn serial_comm_hub_requests__Result__conversion() {
        use generated::types::serial_comm_hub_requests::Result;

        // Test with invalid input
        let error_input = [
            Result {
                status_code: StatusCodeEnum::Error,
                value: None,
            },
            Result {
                status_code: StatusCodeEnum::Success,
                value: None,
            },
            Result {
                status_code: StatusCodeEnum::Success,
                value: Some(vec![1, 2, 3]),
            },
        ];

        for input in error_input {
            assert!(Result::into_vec(input.clone(), 2).is_err());
            assert!(<Result as Into<anyhow::Result<[u16; 2]>>>::into(input).is_err());
        }

        // Test with valid input.
        let correct_input = [
            (
                Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![]),
                },
                vec![],
            ),
            (
                Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![1, 2, 3]),
                },
                vec![1, 2, 3],
            ),
        ];

        for (input, expected) in correct_input.iter() {
            let output = Result::into_vec(input.clone(), expected.len()).unwrap();
            assert_eq!(output, *expected);
        }

        // Test for arrays.
        let output: [u16; 0] =
            <Result as Into<anyhow::Result<[u16; 0]>>>::into(correct_input[0].0.clone()).unwrap();
        assert_eq!(output.len(), 0);

        let output: [u16; 3] =
            <Result as Into<anyhow::Result<[u16; 3]>>>::into(correct_input[1].0.clone()).unwrap();
        assert_eq!(output, [1, 2, 3]);
    }

    #[test]
    fn retry__retry() {
        // Test the fail case without retrying
        let res = retry::retry(|| anyhow::bail!("Failure"), Duration::from_secs(1));
        assert!(res.is_err());

        // Test the timeout case
        let res = retry::retry(
            || Ok(retry::RetryState::KeepTrying),
            Duration::from_millis(5),
        );
        assert!(res.is_err());

        // Test the success case without retry
        let res = retry::retry(|| Ok(retry::RetryState::Done), Duration::from_secs(1));
        assert!(res.is_ok());

        // Test the success after retry.
        let counter = Mutex::new(0);
        let res = retry::retry(
            || {
                if *counter.lock().unwrap() == 1 {
                    return Ok(retry::RetryState::Done);
                }
                *counter.lock().unwrap() += 1;
                Ok(retry::RetryState::KeepTrying)
            },
            Duration::from_secs(1),
        );
        assert!(res.is_ok());
    }

    #[test]
    fn ready_state__write_metadata() {
        let mut mock = SerialCommunicationHubClientPublisher::default();

        mock.expect_modbus_write_multiple_registers()
            .times(1)
            .returning(|_, _, _| Ok(StatusCodeEnum::Success));

        mock.expect_modbus_write_single_register()
            .with(eq(189), eq(7056), eq(1234))
            .times(1)
            .returning(|_, _, _| Ok(StatusCodeEnum::Success));

        mock.expect_modbus_read_holding_registers()
            .with(eq(7100), eq(95), eq(1234))
            .times(1)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![u16::from_be_bytes([b' ', b' ']) as i64; 95]),
                })
            });

        let ready_state = make_ready_state(mock);
        ready_state.write_metadata("some evse id").unwrap();
    }

    #[test]
    fn ready_state__read_signed_meter_values() {
        let mut mock = SerialCommunicationHubClientPublisher::default();

        mock.expect_modbus_read_holding_registers()
            .with(eq(7057), eq(1), eq(1234))
            .times(1)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![15]),
                })
            });

        mock.expect_modbus_read_holding_registers()
            .with(eq(7612), eq(8), eq(1234))
            .times(1)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![u16::from_be_bytes([b'a', b'b']) as i64; 8]),
                })
            });

        let ready_state = make_ready_state(mock);
        let res = ready_state.read_signed_meter_values().unwrap();
        assert_eq!(res, "abababababababa");
    }

    #[test]
    fn ready_state__read_signature() {
        let mut mock = SerialCommunicationHubClientPublisher::default();

        mock.expect_modbus_read_holding_registers()
            .with(eq(7058), eq(1), eq(1234))
            .times(1)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![9]),
                })
            });

        mock.expect_modbus_read_holding_registers()
            .with(eq(8188), eq(5), eq(1234))
            .times(1)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![0xdead, 0xbeef, 0xabcd, 0x1234, 0x5678]),
                })
            });

        let ready_state = make_ready_state(mock);
        let res = ready_state.read_signature().unwrap();
        assert_eq!(res, "DEADBEEFABCD123456");
    }

    #[test]
    fn ready_state__check_signature_status() {
        let parameters = [(15, true), (16, false)];
        for (input, expected) in parameters {
            let mut mock = SerialCommunicationHubClientPublisher::default();
            mock.expect_modbus_read_holding_registers()
                .with(eq(7052), eq(1), eq(1234))
                .times(1)
                .returning(move |_, _, _| {
                    Ok(generated::types::serial_comm_hub_requests::Result {
                        status_code: StatusCodeEnum::Success,
                        value: Some(vec![input as i64]),
                    })
                });

            let ready_state = make_ready_state(mock);
            assert_eq!(ready_state.check_signature_status().is_ok(), expected);
        }
    }

    #[test]
    /// Test verifies that when we try to start the transaction and the meter
    /// is already running a transaction, that we stop the ongoing transaction
    /// before proceeding.
    fn ready_state__start_transaction__try_stop() {
        // The values correspond to the three `Active` values of Iskra.
        for value in [
            IskraMaterState::Active as u8,
            IskraMaterState::Active_after_reset as u8,
        ] {
            let mut mock = SerialCommunicationHubClientPublisher::default();
            // We expect that this is called twice - once in the start_transaction
            // and once in the stop_transaction.
            mock.expect_modbus_read_holding_registers()
                .with(eq(7000), eq(1), eq(1234))
                .times(2)
                .returning(move |_, _, _| {
                    Ok(generated::types::serial_comm_hub_requests::Result {
                        status_code: StatusCodeEnum::Success,
                        value: Some(vec![value as i64]),
                    })
                });

            // This is the call to stop the transaction. We return error to abort
            // the further sequence.
            mock.expect_modbus_write_single_register()
                .with(eq(0x7200), eq(7051), eq(1234))
                .returning(|_, _, _| Ok(StatusCodeEnum::Error));

            let ready_state = make_ready_state(mock);

            let _unused = ready_state.start_transaction(TransactionReq {
                cable_id: 1,
                client_id: String::new(),
                evse_id: String::new(),
                tariff_id: 1,
                transaction_id: String::new(),
                user_data: String::new(),
            });
        }
    }

    #[test]
    fn ready_state__read_public_key() {
        let mut mock = SerialCommunicationHubClientPublisher::default();
        // We expect times(1) since the other calls should be cached.
        mock.expect_modbus_read_holding_registers()
            .with(eq(8124), eq(32), eq(1234))
            .times(1)
            .returning(move |_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![0; 32]),
                })
            });

        let ready_state = make_ready_state(mock);
        let expected = "3059301306072A8648CE3D020106082A8648CE3D0301070342000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000".to_string();
        for _ in 0..2 {
            assert_eq!(ready_state.read_public_key().unwrap(), expected);
        }
    }

    #[test]
    /// Test verifies that when we try to start the transaction and the meter
    /// is already running a transaction, that we stop the ongoing transaction
    /// before proceeding.
    fn ready_state__start_transaction__try_stop_after_power_failure() {
        let mut mock = SerialCommunicationHubClientPublisher::default();
        use mockall::Sequence;
        let mut seq = Sequence::new();

        // First call to read the meter state before starting the transaction
        mock.expect_modbus_read_holding_registers()
            .with(eq(7000), eq(1), eq(1234))
            .times(1)
            .in_sequence(&mut seq)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![IskraMaterState::Active_after_power_failure as i64]),
                })
            });

        // Writes needed to set the time
        for value in [7054, 7055, 7053, 7071] {
            mock.expect_modbus_write_single_register()
                .withf(move |_data, addr, _id| *addr == value as i64)
                .times(1)
                .in_sequence(&mut seq)
                .returning(|_, _, _| Ok(StatusCodeEnum::Success));
        }

        // Writes needed to write metadata
        mock.expect_modbus_write_multiple_registers()
            .withf(|_data, addr, _id| *addr == 7100)
            .times(1)
            .in_sequence(&mut seq)
            .returning(|_, _, _| Ok(StatusCodeEnum::Success));
        mock.expect_modbus_write_single_register()
            .withf(|_data, addr, _id| *addr == 7056)
            .times(1)
            .in_sequence(&mut seq)
            .returning(|_, _, _| Ok(StatusCodeEnum::Success));
        mock.expect_modbus_read_holding_registers()
            .with(eq(7100), eq(89), eq(1234))
            .times(1)
            .in_sequence(&mut seq)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Success,
                    value: Some(vec![u16::from_be_bytes([b' ', b' ']) as i64; 89]),
                })
            });

        // Second call to read the meter state when stopping the transaction
        mock.expect_modbus_read_holding_registers()
            .with(eq(7000), eq(1), eq(1234))
            .times(1)
            .in_sequence(&mut seq)
            .returning(|_, _, _| {
                Ok(generated::types::serial_comm_hub_requests::Result {
                    status_code: StatusCodeEnum::Error,
                    value: Some(vec![IskraMaterState::Idle as i64]),
                })
            });

        let ready_state = make_ready_state(mock);

        let _unused = ready_state.start_transaction(TransactionReq {
            cable_id: 1,
            client_id: String::new(),
            evse_id: String::new(),
            tariff_id: 1,
            transaction_id: String::new(),
            user_data: String::new(),
        });
    }
}
