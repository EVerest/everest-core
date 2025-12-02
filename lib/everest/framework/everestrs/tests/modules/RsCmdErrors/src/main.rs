//! Integration test for the "cmd-errors" handling.
//!
//! The Rust binding can receive/return errors from command calls. The
//! exceptions need the `forward_exceptions` setting. Then errors from the
//! server should propagate to the client.
//!
//! Below we test all possible errors supported by EVerest. In user code it
//! only `HandlerException` typically makes sense.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{
    Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module, ModulePublisher,
    OnReadySubscriber,
};
use std::sync::{Arc, OnceLock};
use std::{thread, time};

pub struct OneClass {
    publisher: OnceLock<ModulePublisher>,
}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, _context: &Context, key: String) -> ::everestrs::Result<bool> {
        match key.as_str() {
            "MessageParsingError" => Err(::everestrs::Error::MessageParsingError(
                "this message?".to_string(),
            )),
            "SchemaValidationError" => Err(::everestrs::Error::SchemaValidationError(
                "not my schema".to_string(),
            )),
            "HandlerException" => Err(::everestrs::Error::HandlerException(
                "my handler".to_string(),
            )),
            "CmdTimeout" => Err(::everestrs::Error::CmdTimeout("no time".to_string())),
            "Shutdown" => Err(::everestrs::Error::Shutdown("dead".to_string())),
            "NotReady" => Err(::everestrs::Error::NotReady("too soon".to_string())),
            _ => Ok(true),
        }
    }
}

// The compilation test is that we don't generate the method interfaces for
// the ignored methods.
impl ExampleClientSubscriber for OneClass {}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, publishers: &ModulePublisher) {
        let _ = self.publisher.set(publishers.clone());
    }
}

fn main() {
    let one_class = Arc::new(OneClass {
        publisher: OnceLock::new(),
    });
    let _module = Module::new(one_class.clone(), one_class.clone(), one_class.clone());
    log::info!("Module initialized");

    let publisher = one_class.publisher.wait();
    for (key, _expected) in [
        (
            "HandlerException",
            Err(::everestrs::Error::HandlerException(String::new())),
        ),
        ("foo", Ok(true)),
        (
            "SchemaValidationError",
            Err(::everestrs::Error::SchemaValidationError(String::new())),
        ),
        (
            "MessageParsingError",
            Err(::everestrs::Error::MessageParsingError(String::new())),
        ),
        (
            "CmdTimeout",
            Err(::everestrs::Error::CmdTimeout(String::new())),
        ),
        ("Shutdown", Err(::everestrs::Error::Shutdown(String::new()))),
        ("NotReady", Err(::everestrs::Error::NotReady(String::new()))),
    ] {
        let res = publisher.other.uses_something(key.to_string());
        assert!(matches!(res, _expected));
    }
    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
