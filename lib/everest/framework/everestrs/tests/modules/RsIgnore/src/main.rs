//! Integration test for the "ignore" handling.
//!
//! The Rust binding allow you to ignore variables and errors. The variables
//! are ignored by adding them to the "ignore.vars" list. Errors can only be ignored
//! at bulk, by setting "ignore.errors" to true. Ignored elements are removed
//! from the trait and thus don't have to be implemented.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::errors::example::{Error as ExampleError, ExampleErrorsError};
use generated::{
    Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module, ModulePublisher,
    OnReadySubscriber,
};
use std::sync::Arc;
use std::{thread, time};

pub struct OneClass {}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, _context: &Context, _key: String) -> ::everestrs::Result<bool> {
        Ok(true)
    }
}

// The compilation test is that we don't generate the method interfaces for
// the ignored methods.
impl ExampleClientSubscriber for OneClass {}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, publishers: &ModulePublisher) {
        // Call the other module. This calls should be ignored.
        publishers.example.max_current(12.3).unwrap();
        let error = ExampleError::ExampleErrors(ExampleErrorsError::ExampleErrorA);
        publishers.example.raise_error(error.clone().into());
        publishers.example.clear_error(error);
    }
}

fn main() {
    let one_class = Arc::new(OneClass {});
    let _module = Module::new(one_class.clone(), one_class.clone(), one_class.clone());
    log::info!("Module initialized");

    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
