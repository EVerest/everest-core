// EVerest expects binaries to be CamelCased, and Rust wants them to be snake_case. We yield to
// EVerest and shut up the compiler warning.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use chrono::{TimeDelta, Utc};
use cron::Schedule;
use generated::{get_config, Context, ExternalEnergyLimitsClientSubscriber, Module, ModuleConfig, ModulePublisher, OnReadySubscriber};
use generated::types::energy::EnforcedLimits;
use openleadr_wire::program::ProgramId;
use openleadr_wire::target::TargetType;
use std::str::FromStr;
use std::sync::Arc;
use std::time::Duration;
use std::{thread, time};
use openleadr_client::{Client, ClientCredentials, Filter, ProgramClient, Timeline};

pub struct OpenleadrClient {
    _client: Client,
    filter: Option<(TargetType, Vec<String>)>,
    schedule: Schedule,
    program: Arc<ProgramClient>,
    rt: Arc<tokio::runtime::Runtime>,
}

impl OnReadySubscriber for OpenleadrClient {
    fn on_ready(&self, publishers: &ModulePublisher) {
        let publisher = publishers.limits_iface.clone();
        let program = self.program.clone();
        let filter = self.filter.clone();
        let schedule = self.schedule.clone();

        self.rt.spawn(async move {
            let program = program;
            let publisher = publisher;
            let filter = filter;
            let schedule = schedule;

            let filter_vec = filter.as_ref().map(|(target_type, target_values)| {
                (target_type, target_values.iter().map(|value| value.as_str()).collect::<Vec<&str>>())
            });


            let upcoming_event_retrieves = schedule.upcoming(Utc);

            for event in upcoming_event_retrieves {

                let mut time_remaining = Utc::now() - event;

                // sleep in maximum steps of 5 minutes, to make sure that power
                // management features don't send our retrieval schedule out of
                // sync
                while time_remaining > TimeDelta::zero() {
                    // we can safely unwrap because our time delta is greater than zero]
                    let duration = time_remaining.to_std().unwrap();

                    if (duration > Duration::from_secs(300)) {
                        // we don't want to sleep for more than 5 minutes
                        tokio::time::sleep(Duration::from_secs(300)).await;
                    } else {
                        // we now sleep our tokio task until the next event
                        tokio::time::sleep(duration).await;
                    }
                    time_remaining = Utc::now() - event;
                }

                // we should be around our expected retrieval time
                let filter_data = if let Some((target_type, target_values)) = &filter_vec {
                    Filter::By((*target_type).clone(), &target_values)
                } else {
                    Filter::None
                };

                let timeline = program.as_ref().get_timeline(filter_data).await.unwrap();

                // TODO: handle events from the timeline
            }


        });
    }
}

impl ExternalEnergyLimitsClientSubscriber for OpenleadrClient {
    fn on_enforced_limits(&self, _context: &Context, _value: EnforcedLimits) {
        // do nothing
    }
}

fn get_credentials_from_config(config: &ModuleConfig) -> Option<ClientCredentials> {
    if !config.openadr_vtn_client_id.is_empty() && !config.openadr_vtn_client_secret.is_empty() {
        Some(ClientCredentials::new(
            config.openadr_vtn_client_id.clone(),
            config.openadr_vtn_client_secret.clone(),
        ))
    } else {
        None
    }
}

fn get_filter_from_config(config: &ModuleConfig) -> Option<(TargetType, Vec<String>)> {
    if !config.openadr_vtn_event_target_type.is_empty() && !config.openadr_vtn_event_target_values.is_empty() {
        let target_type = match config.openadr_vtn_event_target_type.to_uppercase().replace("-", "_").as_str() {
            "POWER_SERVICE_LOCATION" => TargetType::PowerServiceLocation,
            "SERVICE_AREA" => TargetType::ServiceArea,
            "GROUP" => TargetType::Group,
            "RESOURCE_NAME" => TargetType::ResourceName,
            "VEN_NAME" => TargetType::VENName,
            "EVENT_NAME" => TargetType::EventName,
            "PROGRAM_NAME" => TargetType::ProgramName,
            // TODO: maybe do a warning here instead?
            _ => return None,
        };
        Some((target_type, config.openadr_vtn_event_target_values.split(',').map(|s| s.to_string()).collect()))
    } else {
        None
    }
}

fn main() {
    let config = get_config();
    let rt = tokio::runtime::Builder::new_current_thread().build().unwrap();

    // create openleadr-client
    let credentials = get_credentials_from_config(&config);
    let client = Client::with_url(config.openadr_vtn_url.parse().unwrap(), credentials);
    let program_id = ProgramId::new(&config.openadr_vtn_program_id).unwrap();
    let program = rt.block_on(client.get_program_by_id(&program_id)).unwrap();
    let filter = get_filter_from_config(&config);
    let schedule = Schedule::from_str(&config.openadr_vtn_retrieve_schedule).unwrap();
    let adr_client = Arc::new(OpenleadrClient {
        _client: client,
        filter,
        schedule,
        program: Arc::new(program),
        rt: Arc::new(rt),
    });

    // construct the module
    let _module = Module::new(
        adr_client.clone(),
        adr_client.clone(),
    );

    // Everest is driving execution in the background for us, nothing to do.
    loop {
        thread::sleep(time::Duration::from_millis(1000));
    }
}
