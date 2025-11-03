#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{
    Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module,
    ModulePublisher, OnReadySubscriber,
};
use std::sync::Arc;
use std::{thread, time};

pub struct OptionalConnection {}

impl ExampleServiceSubscriber for OptionalConnection {
    fn uses_something(&self, _context: &Context, key: String) -> ::everestrs::Result<bool> {
        log::info!("Received {key}");
        Ok(&key == "hello")
    }
}

impl ExampleClientSubscriber for OptionalConnection {
    fn on_max_current(&self, _context: &Context, value: f64) {
        log::info!("Received {value}");
    }

    fn on_error_raised(&self, _context: &Context, error: crate::generated::errors::example::Error) {
        log::info!("Recieved an error {error:?}");
    }

    fn on_error_cleared(
        &self,
        _context: &Context,
        error: crate::generated::errors::example::Error,
    ) {
        log::info!("Cleared an error {error:?} - what a relief");
    }
}

impl OnReadySubscriber for OptionalConnection {
    fn on_ready(&self, publishers: &ModulePublisher) {
        log::info!("Ready");
        if let Some(publisher) = publishers.optional_connection_slots.get(0) {
            let res = publisher.uses_something("hello".to_string()).unwrap();
            assert!(res);
        }
    }
}

fn main() {
    let one_class = Arc::new(OptionalConnection {});
    let _module = Module::new(one_class.clone(), one_class.clone(), |_index| {one_class.clone()});
    log::info!("Module initialized");

    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
