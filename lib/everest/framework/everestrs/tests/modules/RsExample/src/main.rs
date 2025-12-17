#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use everestrs::ErrorType;
use generated::errors::example::Error as ExampleError;
use generated::{
    get_config, Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module,
    ModulePublisher, OnReadySubscriber,
};
use std::sync::Arc;
use std::{thread, time};

pub struct OneClass {}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, context: &Context, key: String) -> ::everestrs::Result<bool> {
        use crate::generated::errors::example::ExampleErrorsError;
        let error = ExampleError::ExampleErrors(ExampleErrorsError::ExampleErrorA);
        if key.is_empty() {
            // Explicit cast
            let error: ErrorType<_> = error.into();
            context.publisher.foobar.raise_error(error);
        } else if &key == "clear_all" {
            context.publisher.foobar.clear_all_errors();
        } else {
            context.publisher.foobar.clear_error(error);
        }

        Ok(true)
    }
}

impl ExampleClientSubscriber for OneClass {
    fn on_max_current(&self, _context: &Context, value: f64) {
        log::info!("Received {value}");
    }

    fn on_error_raised(&self, _context: &Context, error: ErrorType<ExampleError>) {
        log::warn!("Recieved an error {:?}", error.error_type);
    }

    fn on_error_cleared(&self, _context: &Context, error: ErrorType<ExampleError>) {
        log::info!("Cleared an error {:?} - what a relief", error.error_type);
    }
}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, publishers: &ModulePublisher) {
        log::info!("Ready");
        match publishers.foobar.max_current(123.0) {
            Ok(_) => log::info!("Adjusted the max current"),
            Err(err) => log::error!("Failed to set the max current: {err:?}"),
        }
    }
}

fn main() {
    let config = get_config();
    log::info!("Received the config {config:?}");
    let one_class = Arc::new(OneClass {});
    let _module = Module::new(one_class.clone(), one_class.clone(), one_class.clone());
    log::info!("Module initialized");

    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}

#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn test_on_ready() {
        let mut everest_mock = ModulePublisher::default();
        everest_mock
            .foobar
            .expect_max_current()
            .times(1)
            .return_once(|_| Ok(()));

        let module = OneClass {};
        module.on_ready(&everest_mock);
    }

    #[test]
    fn test_uses_something() {
        use mockall::Sequence;

        let mut seq = Sequence::new();
        let mut everest_mock = ModulePublisher::default();

        everest_mock
            .foobar
            .expect_raise_error()
            .times(1)
            .in_sequence(&mut seq)
            .return_once(|_| ());

        everest_mock
            .foobar
            .expect_clear_error()
            .times(1)
            .in_sequence(&mut seq)
            .return_once(|_| ());

        everest_mock
            .foobar
            .expect_clear_all_errors()
            .times(1)
            .in_sequence(&mut seq)
            .return_once(|| ());

        let context = Context {
            name: "foo",
            publisher: &everest_mock,
            index: 0,
        };

        let module = OneClass {};
        for message in [String::new(), "clear".to_owned(), "clear_all".to_owned()] {
            let _ = module.uses_something(&context, message);
        }
    }
}
