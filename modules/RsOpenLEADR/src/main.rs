// EVerest expects binaries to be CamelCased, and Rust wants them to be snake_case. We yield to
// EVerest and shut up the compiler warning.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{get_config, Context, ExternalEnergyLimitsClientSubscriber, Module, ModuleConfig, ModulePublisher, OnReadySubscriber};
use generated::types::energy::EnforcedLimits;
use openleadr_wire::program::ProgramId;
use std::sync::Arc;
use std::{thread, time};
use openleadr_client::{Client, ClientCredentials, ProgramClient};

pub struct OpenleadrClient {
    client: Client,
    program: ProgramClient,
    rt: Arc<tokio::runtime::Runtime>,
}

impl OnReadySubscriber for OpenleadrClient {
    fn on_ready(&self, publishers: &ModulePublisher) {
        // publishers.limits_iface.set_external_limits();
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

fn main() {
    let config = get_config();
    let rt = tokio::runtime::Builder::new_current_thread().build().unwrap();

    // create openleadr-client
    let credentials = get_credentials_from_config(&config);
    let client = Client::with_url(config.openadr_vtn_url.parse().unwrap(), credentials);
    let program_id = ProgramId::new(&config.openadr_vtn_program_id).unwrap();
    let program = rt.block_on(client.get_program_by_id(&program_id)).unwrap();
    let adr_client = Arc::new(OpenleadrClient {
        client,
        program,
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
