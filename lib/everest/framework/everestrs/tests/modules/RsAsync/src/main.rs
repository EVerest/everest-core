#![allow(non_snake_case)]
include!(concat!(env!("OUT_DIR"), "/generated.rs"));

use generated::{
    Context, ExampleClientSubscriber, ExampleServiceSubscriber, Module, ModulePublisher,
    OnReadySubscriber,
};
use std::sync::{Arc, Mutex};
// Just to use async.
use tokio::sync::oneshot;
use tokio::time::sleep;

pub struct OneClass {
    tx: Mutex<Option<oneshot::Sender<String>>>,
}

impl ExampleServiceSubscriber for OneClass {
    fn uses_something(&self, _context: &Context, key: String) -> ::everestrs::Result<bool> {
        log::info!("Received {key}");
        let tx = self.tx.lock().unwrap().take();
        if let Some(tx) = tx {
            tx.send(key).unwrap();
        }
        Ok(true)
    }
}

impl ExampleClientSubscriber for OneClass {
    fn on_max_current(&self, context: &Context, value: f64) {
        log::info!("Received {value}");
        context
            .publisher
            .receiver
            .uses_something(format!("{value}"))
            .unwrap();
    }
}

impl OnReadySubscriber for OneClass {
    fn on_ready(&self, _publishers: &ModulePublisher) {
        log::info!("Ready");
    }
}

/// Example how to use async with Everest. Everything in EVerest (all traits)
/// remain strictly sync because of the underlying c++ runtime. However, you can
/// combine your async code with sync EVerest.
///
/// You can combine the `everestrs::main` macro with `tokio::main` macro. The
/// ordering does not really matter, so for non-main function (functions which
/// can receive input args), you can also write
/// ```ignore
/// #[tokio::main]
/// #[everestrs::main]
/// async fn my_fun(module: &Module) {}
/// ```
#[everestrs::main]
#[tokio::main]
async fn main(module: &Module) {
    let config = module.get_config();
    log::info!("Received the config {config:?}");
    let (tx, rx) = oneshot::channel();
    let one_class = Arc::new(OneClass {
        tx: Mutex::new(Some(tx)),
    });
    let publishers = module.start(one_class.clone(), one_class.clone(), one_class.clone());

    publishers.sender.max_current(123.).unwrap();

    // Simulate some async steps...
    let result = rx.await.unwrap();
    log::info!("Done {result}");

    loop {
        sleep(std::time::Duration::from_secs(1)).await;
    }
}
