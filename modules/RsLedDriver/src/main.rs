#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{get_config, Context, Module, ModulePublisher};
use generated::{
    EmptyServiceSubscriber, EvseManagerClientSubscriber, LedDriverClientSubscriber,
    OnReadySubscriber,
};

use generated::types::energy::EnforcedLimits;
use generated::types::evse_board_support::{HardwareCapabilities, Telemetry};
use generated::types::evse_manager::*;
use generated::types::iso15118_charger::RequestExiStreamSchema;
use generated::types::led_state::LedState;
use generated::types::powermeter::Powermeter;

use std::sync::Arc;
use std::{thread::sleep, time::Duration};

fn get_led_state(trigger: SessionEventEnum) -> Option<LedState> {
    match trigger {
        SessionEventEnum::Disabled
        | SessionEventEnum::AuthRequired
        | SessionEventEnum::PluginTimeout => Some(LedState {
            red: 255,
            green: 0,
            blue: 0,
        }),

        // Blue triggers
        SessionEventEnum::Authorized
        | SessionEventEnum::TransactionStarted
        | SessionEventEnum::ChargingStarted
        | SessionEventEnum::ChargingPausedEV
        | SessionEventEnum::ChargingPausedEVSE
        | SessionEventEnum::ChargingResumed
        | SessionEventEnum::StoppingCharging
        | SessionEventEnum::ChargingFinished
        | SessionEventEnum::SessionResumed => Some(LedState {
            red: 0,
            green: 0,
            blue: 255,
        }),

        // Green triggers
        SessionEventEnum::Deauthorized
        | SessionEventEnum::Enabled
        | SessionEventEnum::ReservationStart
        | SessionEventEnum::ReservationEnd
        | SessionEventEnum::SessionFinished
        | SessionEventEnum::TransactionFinished => Some(LedState {
            red: 0,
            green: 255,
            blue: 0,
        }),
        _ => None,
    }
}

pub struct LedDriver {
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
        let led_driver = context.publisher.led_driver_slots[index].clone();

        let _ = match get_led_state(value.event.clone()) {
            Some(led_state) => led_driver
                .set_led_state(self.brightness, led_state)
                .unwrap(),
            None => log::error!("Unknown event {:?} - not changing LEDs", value.event),
        };
    }

    fn on_telemetry(&self, _context: &Context, _value: Telemetry) {}

    fn on_waiting_for_external_ready(&self, _context: &Context, _value: bool) {}

    fn on_powermeter_public_key_ocmf(&self, _context: &Context, _value: String) {}
}

impl OnReadySubscriber for LedDriver {
    fn on_ready(&self, publishers: &ModulePublisher) {
        if publishers.evse_manager_slots.len() != publishers.led_driver_slots.len() {
            panic!("EVSE slots and LED driver slots are not the same size!");
        }
    }
}

fn main() {
    let config = get_config();

    let led_driver = Arc::new(LedDriver {
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
