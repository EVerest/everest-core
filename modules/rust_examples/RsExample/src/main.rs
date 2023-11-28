// EVerest expects binaries to be CamelCased, and Rust wants them to be snake_case. We yield to
// EVerest and shut up the compiler warning.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{
    get_config, ExampleServiceSubscriber, KvsClientSubscriber, KvsServiceSubscriber, Module,
    ModulePublisher, OnReadySubscriber,
};
use std::sync::Arc;
use std::{thread, time};

pub struct OneClass {}

impl KvsServiceSubscriber for OneClass {
    fn store(
        &self,
        pub_impl: &ModulePublisher,
        key: String,
        value: serde_json::Value,
    ) -> ::everestrs::Result<()> {
        pub_impl.their_store.store(key, value)
    }

    fn load(
        &self,
        pub_impl: &ModulePublisher,
        key: String,
    ) -> ::everestrs::Result<serde_json::Value> {
        pub_impl.their_store.load(key)
    }

    fn delete(&self, pub_impl: &ModulePublisher, key: String) -> ::everestrs::Result<()> {
        pub_impl.their_store.delete(key)
    }

    fn exists(&self, pub_impl: &ModulePublisher, key: String) -> ::everestrs::Result<bool> {
        pub_impl.their_store.exists(key)
    }
}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, pub_impl: &ModulePublisher, key: String) -> ::everestrs::Result<bool> {
        if !pub_impl.their_store.exists(key.clone())? {
            println!("IT SHOULD NOT AND DOES NOT EXIST");
        }

        let test_array = vec![1, 2, 3];
        pub_impl
            .their_store
            .store(key.clone(), test_array.clone().into())?;

        let exi = pub_impl.their_store.exists(key.clone())?;
        if exi {
            println!("IT ACTUALLY EXISTS");
        }

        let ret: Vec<i32> = serde_json::from_value(pub_impl.their_store.load(key)?)
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
