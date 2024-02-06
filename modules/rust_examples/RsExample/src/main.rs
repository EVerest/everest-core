// EVerest expects binaries to be CamelCased, and Rust wants them to be snake_case. We yield to
// EVerest and shut up the compiler warning.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{
    get_config, Context, ExampleServiceSubscriber, KvsClientSubscriber, KvsServiceSubscriber,
    Module, ModulePublisher, OnReadySubscriber,
};
use std::sync::Arc;
use std::{thread, time};

pub struct OneClass {}

impl KvsServiceSubscriber for OneClass {
    fn store(
        &self,
        context: &Context,
        key: String,
        value: serde_json::Value,
    ) -> ::everestrs::Result<()> {
        context.publisher.their_store.store(key, value)
    }

    fn load(&self, context: &Context, key: String) -> ::everestrs::Result<serde_json::Value> {
        context.publisher.their_store.load(key)
    }

    fn delete(&self, context: &Context, key: String) -> ::everestrs::Result<()> {
        context.publisher.their_store.delete(key)
    }

    fn exists(&self, context: &Context, key: String) -> ::everestrs::Result<bool> {
        context.publisher.their_store.exists(key)
    }
}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, context: &Context, key: String) -> ::everestrs::Result<bool> {
        if !context.publisher.their_store.exists(key.clone())? {
            println!("IT SHOULD NOT AND DOES NOT EXIST");
        }

        let test_array = vec![1, 2, 3];
        context
            .publisher
            .their_store
            .store(key.clone(), test_array.clone().into())?;

        let exi = context.publisher.their_store.exists(key.clone())?;
        if exi {
            println!("IT ACTUALLY EXISTS");
        }

        let ret: Vec<i32> = serde_json::from_value(context.publisher.their_store.load(key)?)
            .expect("Wanted an array as return value");

        println!("loaded array: {ret:?}, original array: {test_array:?}");
        Ok(exi)
    }
}

impl KvsClientSubscriber for OneClass {}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, publishers: &ModulePublisher) {
        // Ignore errors here.
        let _ = publishers.foobar.max_current(125.);
    }
}

fn main() {
    let config = get_config();
    println!("Received the config {config:?}");
    let one_class = Arc::new(OneClass {});
    let _module = Module::new(
        one_class.clone(),
        one_class.clone(),
        one_class.clone(),
        one_class.clone(),
    );
    // Everest is driving execution in the background for us, nothing to do.
    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
