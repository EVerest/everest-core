use everestrs::{Error, Result, Runtime, Subscriber};
use std::collections::HashMap;
use std::sync::Arc;

/// Trait for the user to implement.
pub trait OnReadySubscriber: Sync + Send {
    fn on_ready(&self, publishers: &ModulePublisher);
}

/// Generated from `manifest.provides.main`. At best we will omit empty traits.
pub trait ExampleUserServiceSubscriber: Sync + Send {}

/// Generated from `manifest.provides.main`. At best we will omit empty structs.
#[derive(Clone)]
pub struct ExampleUserServicePublisher {
    #[allow(unused)]
    runtime: Arc<Runtime>,
}

impl ExampleUserServicePublisher {}

/// Generated from `manifest.requires.their_example` and
/// `manifest.requires.another_example`.
trait ExampleClientSubscriber {
    fn max_current(&self, publishers: &ModulePublisher, max_current: f64);
}

/// The publisher for the example module. The class is clone-able and just holds
/// a shared-ptr to the cpp implementation.
#[derive(Clone)]
pub struct ExampleClientPublisher {
    runtime: Arc<Runtime>,
}

impl ExampleClientPublisher {
    /// This command checks if something is stored under a given key
    ///
    /// `key`: Key to check the existence for
    ///
    /// Returns: Returns 'True' if something was stored for this key
    pub fn uses_something(&self, key: String) -> Result<bool> {
        let args = serde_json::json!({
            "key": key,
        });
        let blob = self
            .runtime
            .call_command("their_example", "uses_something", &args);
        let return_value: bool =
            ::serde_json::from_value(blob).map_err(|_| Error::InvalidArgument("return_value"))?;
        Ok(return_value)
    }
}

/// The collection of all publishers for this module.
#[derive(Clone)]
pub struct ModulePublisher {
    pub main: ExampleUserServicePublisher,
    pub their_example: ExampleClientPublisher,
    pub another_example: ExampleClientPublisher,
}

/// Trait for the user to implement.
pub trait ExampleSubscriber: Sync + Send {
    fn on_max_current(&self, publishers: &ModulePublisher, value: f64);
}

/// The struct holding everything necessary for the module to run.
///
/// If the user may drop the provided subscriber and the code will still work.
/// The user may furthermore drop the [Module] - in this case we will stop
/// receiving callbacks - however the cloned publisher will continue to work
/// until dropped.
#[allow(dead_code)]
pub struct Module {
    /// All subscribers.
    on_ready: Arc<dyn OnReadySubscriber>,
    main: Arc<dyn ExampleUserServiceSubscriber>,
    their_example: Arc<dyn ExampleSubscriber>,
    another_example: Arc<dyn ExampleSubscriber>,

    /// The publisher.
    publisher: ModulePublisher,
}

impl Module {
    #[must_use]
    pub fn new(
        on_ready: Arc<dyn OnReadySubscriber>,
        main: Arc<dyn ExampleUserServiceSubscriber>,
        their_example: Arc<dyn ExampleSubscriber>,
        another_example: Arc<dyn ExampleSubscriber>,
    ) -> Arc<Self> {
        let runtime = Arc::new(Runtime::new());
        let publisher = ModulePublisher {
            main: ExampleUserServicePublisher {
                runtime: runtime.clone(),
            },
            their_example: ExampleClientPublisher {
                runtime: runtime.clone(),
            },
            another_example: ExampleClientPublisher {
                runtime: runtime.clone(),
            },
        };

        let this = Arc::new(Self {
            on_ready,
            main,
            their_example,
            another_example,
            publisher,
        });
        runtime.set_subscriber(Arc::<Module>::downgrade(&this));
        this
    }
}

impl Subscriber for Module {
    fn handle_command(
        &self,
        _implementation_id: &str,
        _name: &str,
        _parameters: HashMap<String, serde_json::Value>,
    ) -> Result<serde_json::Value> {
        Err(Error::InvalidArgument("Unknown command called received."))
    }

    fn handle_variable(
        &self,
        implementation_id: &str,
        name: &str,
        value: serde_json::Value,
    ) -> Result<()> {
        match implementation_id {
            "their_example" => example_interface::handle_variable(
                &self.publisher,
                self.their_example.as_ref(),
                name,
                value,
            ),
            "another_example" => example_interface::handle_variable(
                &self.publisher,
                self.another_example.as_ref(),
                name,
                value,
            ),
            _ => Err(Error::InvalidArgument("Unknown variable received.")),
        }
    }

    fn on_ready(&self) {
        self.on_ready.on_ready(&self.publisher)
    }
}

mod example_interface {
    // use Runtime;
    use super::*;

    pub(super) fn handle_variable<T: super::ExampleSubscriber + ?Sized>(
        publishers: &super::ModulePublisher,
        their_example_subscriber: &T,
        name: &str,
        value: serde_json::Value,
    ) -> Result<()> {
        match name {
            "max_current" => {
                let v: f64 = ::serde_json::from_value(value)
                    .map_err(|_| Error::InvalidArgument("max_current"))?;
                their_example_subscriber.on_max_current(publishers, v);
                Ok(())
            }
            _ => Err(Error::InvalidArgument("Unknown variable received.")),
        }
    }
}
