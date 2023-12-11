// EVerest expects binaries to be CamelCased, and Rust wants them to be snake_case. We yield to
// EVerest and shut up the compiler warning.
#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use std::sync::{Arc, Mutex};
use std::{thread, time};

use generated::{
    Context, ExampleClientSubscriber, ExampleUserServiceSubscriber, Module, ModulePublisher,
    OnReadySubscriber,
};

struct ExampleClient {
    max_current: Mutex<Option<f64>>,
    thread: Mutex<Option<thread::JoinHandle<()>>>,
}

impl ExampleClient {
    fn new() -> Self {
        Self {
            max_current: Mutex::new(None),
            thread: Mutex::new(None),
        }
    }
}

impl ExampleClientSubscriber for ExampleClient {
    fn on_max_current(&self, context: &Context, value: f64) {
        println!("Received the value {value}");
        let _ = context
            .publisher
            .their_example
            .uses_something("hello_there".to_string());
        *self.max_current.lock().unwrap() = Some(value);

        // Example where we start a thread with the publisher. The cloning is
        // only done if the user wants to offload the publisher into a thread.
        let clone = context.publisher.clone();
        *self.thread.lock().unwrap() = Some(thread::spawn(move || {
            let _ = clone.another_example.uses_something("foo".to_string());
        }))
    }
}

struct MainService {}
impl ExampleUserServiceSubscriber for MainService {}

struct OurModule {
    their_example: Arc<ExampleClient>,
    another_example: Arc<ExampleClient>,
    min_current: Mutex<Option<f64>>,
}

impl OnReadySubscriber for OurModule {
    fn on_ready(&self, _pub_impl: &ModulePublisher) {
        let mut their_current = self.their_example.max_current.lock().unwrap();
        let mut another_current = self.another_example.max_current.lock().unwrap();
        *their_current = Some(1.);
        *another_current = Some(2.);
        // uses somehow both...
        *self.min_current.lock().unwrap() = Some(1.);
    }
}

fn main() {
    let their_example = Arc::new(ExampleClient::new());
    let another_example = Arc::new(ExampleClient::new());
    let main_service = Arc::new(MainService {});
    let our_module = Arc::new(OurModule {
        their_example: their_example.clone(),
        another_example: another_example.clone(),
        min_current: Mutex::new(None),
    });
    let _module = Module::new(
        our_module.clone(),
        main_service.clone(),
        their_example.clone(),
        another_example.clone(),
    );

    // Everest is driving execution in the background for us, nothing to do.
    loop {
        let dt = time::Duration::from_millis(250);
        thread::sleep(dt);
    }
}
