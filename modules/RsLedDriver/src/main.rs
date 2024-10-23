#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{get_config, Context, Module, ModulePublisher};
use generated::{
    EmptyServiceSubscriber, EvseManagerClientSubscriber, LedDriverClientPublisher,
    LedDriverClientSubscriber, OnReadySubscriber,
};

use generated::types::energy::EnforcedLimits;
use generated::types::evse_board_support::{HardwareCapabilities, Telemetry};
use generated::types::evse_manager::*;
use generated::types::iso15118_charger::RequestExiStreamSchema;
use generated::types::led_state::LedState;
use generated::types::powermeter::Powermeter;

use std::sync::{Arc, Mutex};
use std::{thread::sleep, time::Duration};

fn get_led_state(trigger: SessionEventEnum) -> Option<LedState> {
    match trigger {
        SessionEventEnum::Disabled
        | SessionEventEnum::AuthRequired
        | SessionEventEnum::PluginTimeout => Some(LedState::Red),

        // Blue triggers
        SessionEventEnum::TransactionStarted
        | SessionEventEnum::PrepareCharging
        | SessionEventEnum::ChargingStarted
        | SessionEventEnum::ChargingPausedEV
        | SessionEventEnum::ChargingPausedEVSE
        | SessionEventEnum::WaitingForEnergy
        | SessionEventEnum::ChargingResumed
        | SessionEventEnum::StoppingCharging
        | SessionEventEnum::ChargingFinished
        | SessionEventEnum::TransactionFinished
        | SessionEventEnum::SessionResumed => Some(LedState::Blue),

        // Green triggers
        SessionEventEnum::Authorized
        | SessionEventEnum::Deauthorized
        | SessionEventEnum::Enabled
        | SessionEventEnum::SessionFinished
        | SessionEventEnum::ReservationStart
        | SessionEventEnum::ReservationEnd => Some(LedState::Green),
        _ => None,
    }
}

pub struct LedDriver {
    // This vector contains all the led_drivers.
    // Every entry here matches the index of evse_manager in config
    led_drivers: Mutex<Vec<LedDriverClientPublisher>>,
    brightness: i64,
}

impl LedDriverClientSubscriber for LedDriver {}

impl EmptyServiceSubscriber for LedDriver {}

impl EvseManagerClientSubscriber for LedDriver {
    fn on_car_manufacturer(&self, _context: &Context, _value: CarManufacturer) {}

    fn on_enforced_limits(&self, _context: &Context, _value: EnforcedLimits) {}

    fn on_ev_info(&self, _context: &Context, _value: EVInfo) {}

    fn on_evse_id(&self, _context: &Context, _value: String) {}

    fn on_hw_capabilities(&self, _context: &Context, _value: HardwareCapabilities) {}

    fn on_iso_15118_certificate_request(&self, _context: &Context, _value: RequestExiStreamSchema) {
    }

    fn on_limits(&self, _context: &Context, _value: Limits) {}

    fn on_powermeter(&self, _context: &Context, _value: Powermeter) {}

    fn on_ready(&self, _context: &Context, _value: bool) {}

    fn on_selected_protocol(&self, _context: &Context, _value: String) {}

    fn on_session_event(&self, context: &Context, value: SessionEvent) {
        let index = context.index;
        let led_driver = self.led_drivers.lock().unwrap()[index].clone();

        let _ = match get_led_state(value.event) {
            Some(led_state) => led_driver.set_led_state(self.brightness, led_state).unwrap(),
            None => log::error!("Unknown event - not changing LEDs"),
        };
    }

    fn on_telemetry(&self, _context: &Context, _value: Telemetry) {}

    fn on_waiting_for_external_ready(&self, _context: &Context, _value: bool) {}
}

impl OnReadySubscriber for LedDriver {
    fn on_ready(&self, publishers: &ModulePublisher) {
        let led_drivers = &publishers.led_driver_slots;

        if publishers.evse_manager_slots.len() != led_drivers.len() {
            panic!("EVSE slots and LED driver slots are not the same size!");
        }

        let mut led_drivers_mut = self.led_drivers.lock().unwrap();

        led_drivers_mut.clear();
        led_drivers_mut.extend(led_drivers.iter().cloned());
    }
}

fn main() {
    let config = get_config();

    let led_driver = Arc::new(LedDriver {
        led_drivers: Mutex::new(Vec::default()),
        brightness: config.default_brightness,
    });

    let _module = Module::new(
        led_driver.clone(),
        led_driver.clone(),
        |_| led_driver.clone(),
        |_| led_driver.clone(),
    );

    loop {
        sleep(Duration::from_secs(1));
    }
}
