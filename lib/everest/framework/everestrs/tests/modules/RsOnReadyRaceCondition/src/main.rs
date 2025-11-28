//! Integration test for the "ready_received" handling.
//!
//! We assume that every module shall recieve first `on_ready` before forwarding
//! any other call to the user code. The code below recreates the race condition
//! by making calls to the `other` module from within `on_ready` (and adding a
//! delay in `on_ready`).
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use everestrs::ErrorType;
use generated::errors::example::{Error as ExampleError, ExampleErrorsError};
use generated::{
    Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module, ModulePublisher,
    OnReadySubscriber,
};
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::{thread, time};

pub struct OneClass {
    /// Flag that the on-ready has been called.
    on_ready_called: AtomicBool,
}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, _context: &Context, _key: String) -> ::everestrs::Result<bool> {
        assert!(self.on_ready_called.load(Ordering::Relaxed));
        Ok(true)
    }
}

impl ExampleClientSubscriber for OneClass {
    fn on_max_current(&self, _context: &Context, _value: f64) {
        assert!(self.on_ready_called.load(Ordering::Relaxed));
        log::info!("max current");
    }

    fn on_error_raised(&self, _context: &Context, _error: ErrorType<ExampleError>) {
        assert!(self.on_ready_called.load(Ordering::Relaxed));
        log::info!("Error raised");
    }

    fn on_error_cleared(&self, _context: &Context, _error: ErrorType<ExampleError>) {
        assert!(self.on_ready_called.load(Ordering::Relaxed));
        log::info!("Error cleared");
    }
}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, publishers: &ModulePublisher) {
        log::info!("Enter Ready");
        // Call the other module.
        publishers.example.max_current(12.3).unwrap();
        let error = ExampleError::ExampleErrors(ExampleErrorsError::ExampleErrorA);
        publishers.example.raise_error(error.clone().into());
        publishers.example.clear_error(error);

        // TODO(ddo) Add here the `uses_something` call once the framework can
        // reject too early calls.

        // Sleep here to trigger the race condition.
        std::thread::sleep(std::time::Duration::from_secs(1));
        // Update the flag.
        self.on_ready_called.store(true, Ordering::Relaxed);

        log::info!("Exit Ready!");
    }
}

fn main() {
    let one_class = Arc::new(OneClass {
        on_ready_called: AtomicBool::new(false),
    });
    let _module = Module::new(one_class.clone(), one_class.clone(), one_class.clone());
    log::info!("Module initialized");

    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
