//! The things which should be eventually generated from the `manifest`.
//!
//! The naming convention should be:
//! `INTERFACE_NAME` `SERVER|CLIENT` `PUBLISHER|SUBSCRIBER`.
//!
//! The server publisher implements the publisher for the variables. The
//! server subscriber is trait where the user implements the RPC callbacks.
//! The client publisher implements interface for calling the rcps of other
//! servers. The client subscriber is a trait where the user implements
//! callbacks for changed variables.

#![allow(clippy::let_unit_value, clippy::useless_conversion)]

use everestrs::{Error, Result, Runtime, Subscriber};
use std::collections::HashMap;
use std::sync::Arc;

/// The trait for the user to provide. Always part of the generated code.
pub trait OnReadySubscriber: Sync + Send {
    fn on_ready(&self, pub_impl: &ModulePublisher);
}

/// The trait for the user to implement. Generated from the
/// `manifest.provides.foobar`.
pub trait ExampleServiceSubscriber: Sync + Send {
    /// This command checks if something is stored under a given key
    ///
    /// `pub_impl`: Handle to all publishers.
    /// `key`: Key to check the existence for
    ///
    /// Returns: Returns 'True' if something was stored for this key*/
    fn uses_something(&self, pub_impl: &ModulePublisher, key: String) -> Result<bool>;
}

/// The publisher generated from `manifest.provides.foobar`.
#[derive(Clone)]
#[allow(unused)]
pub struct ExampleServicePublisher {
    runtime: Arc<Runtime>,
}

impl ExampleServicePublisher {
    pub fn max_current(&self, max_current: f64) -> Result<()> {
        self.runtime
            .publish_variable("foobar", "max_current", &max_current);
        Ok(())
    }
}

/// The trait for the user to implement. Generated from
/// `manifest.provides.my_store`.
pub trait KvsServiceSubscriber: Sync + Send {
    /// This command removes the value stored under a given key
    ///
    /// `pub_impl`: Handle to all publishers.
    /// `key`: Key to delete the value for
    fn delete(&self, pub_impl: &ModulePublisher, key: String) -> Result<()>;

    /// This command checks if something is stored under a given key
    ///
    /// `pub_impl`: Handle to all publishers.
    /// `key`: Key to check the existence for
    ///
    /// Returns: Returns 'True' if something was stored for this key*/
    fn exists(&self, pub_impl: &ModulePublisher, key: String) -> Result<bool>;

    /// This command loads the previously stored value for a given key (it will return null if the
    /// key does not exist)
    ///
    /// `pub_impl`: Handle to all publishers.
    /// `key`: Key to load the value for
    ///
    /// Returns: The previously stored value
    fn load(&self, pub_impl: &ModulePublisher, key: String) -> Result<serde_json::Value>;

    /// This command stores a value under a given key
    ///
    /// `pub_impl`: Handle to all publishers.
    /// `key`: Key to store the value for
    /// `value`: Value to store
    fn store(
        &self,
        pub_impl: &ModulePublisher,
        key: String,
        value: ::serde_json::Value,
    ) -> Result<()>;
}

/// The publisher for the KvsService. Generated from `manifest.provides.my_store`.
///
/// The Kvs does not publish anything - so this struct is a Noop. Eventually
/// the code generation might be smart enough to "optimize" such things away but
/// here we include it for completeness.
#[derive(Clone)]
pub struct KvsServicePublisher {
    #[allow(unused)]
    runtime: Arc<Runtime>,
}

impl KvsServicePublisher {}

/// The subscriber for the KvsClient. Generated from
/// `manifest.requires.their_store`.
///
/// The Kvs does not publish any variables, therefore the subscriber is empty.
/// Eventually our code generation might be smart enough to omit this, but here
/// we include it for completeness.
pub trait KvsClientSubscriber: Sync + Send {}

/// This interface defines a simple key-value-store interface. Generated for
/// `manifest.requires.their_store`.
#[derive(Clone)]
pub struct KvsClientPublisher {
    runtime: Arc<Runtime>,
}

impl KvsClientPublisher {
    /// This command removes the value stored under a given key
    ///
    /// `key`: Key to delete the value for
    pub fn delete(&self, key: String) -> Result<()> {
        let args = serde_json::json!({
            "key": key,
        });
        let blob = self.runtime.call_command("their_store", "delete", &args);
        ::serde_json::from_value(blob).map_err(|_| Error::InvalidArgument("return_value"))
    }

    /// This command checks if something is stored under a given key
    ///
    /// `key`: Key to check the existence for
    ///
    /// Returns: Returns 'True' if something was stored for this key*/
    pub fn exists(&self, key: String) -> Result<bool> {
        let args = serde_json::json!({
            "key": key,
        });
        let blob = self.runtime.call_command("their_store", "exists", &args);
        ::serde_json::from_value(blob).map_err(|_| Error::InvalidArgument("return_value"))
    }

    /// This command loads the previously stored value for a given key (it will return null if the
    /// key does not exist)
    ///
    /// `key`: Key to load the value for
    ///
    /// Returns: The previously stored value
    pub fn load(&self, key: String) -> Result<serde_json::Value> {
        let args = serde_json::json!({
            "key": key,
        });
        let blob = self.runtime.call_command("their_store", "load", &args);
        ::serde_json::from_value(blob).map_err(|_| Error::InvalidArgument("return_value"))
    }

    /// This command stores a value under a given key
    ///
    /// `key`: Key to store the value for
    /// `value`: Value to store
    pub fn store(&self, key: String, value: ::serde_json::Value) -> Result<()> {
        let args = serde_json::json!({
            "key": key,
            "value": value,
        });
        let blob = self.runtime.call_command("their_store", "store", &args);
        ::serde_json::from_value(blob).map_err(|_| Error::InvalidArgument("return_value"))
    }
}

pub struct ModulePublisher {
    pub foobar: ExampleServicePublisher,
    pub my_store: KvsServicePublisher,
    pub their_store: KvsClientPublisher,
}

#[allow(dead_code)]
pub struct Module {
    /// All subscribers
    on_ready: Arc<dyn OnReadySubscriber>,
    foobar: Arc<dyn ExampleServiceSubscriber>,
    my_store: Arc<dyn KvsServiceSubscriber>,
    their_store: Arc<dyn KvsClientSubscriber>,

    // The publisher
    publisher: ModulePublisher,
}

impl Module {
    #[must_use]
    pub fn new(
        on_ready: Arc<dyn OnReadySubscriber>,
        foobar: Arc<dyn ExampleServiceSubscriber>,
        my_store: Arc<dyn KvsServiceSubscriber>,
        their_store: Arc<dyn KvsClientSubscriber>,
    ) -> Arc<Self> {
        let runtime = Arc::new(Runtime::new());
        let this = Arc::new(Self {
            on_ready,
            foobar,
            my_store,
            their_store,
            publisher: ModulePublisher {
                foobar: ExampleServicePublisher {
                    runtime: runtime.clone(),
                },
                my_store: KvsServicePublisher {
                    runtime: runtime.clone(),
                },
                their_store: KvsClientPublisher {
                    runtime: runtime.clone(),
                },
            },
        });

        runtime.set_subscriber(Arc::<Module>::downgrade(&this));

        this
    }
}

impl Subscriber for Module {
    #[allow(unused_variables)]
    fn handle_command(
        &self,
        implementation_id: &str,
        cmd_name: &str,
        parameters: HashMap<String, serde_json::Value>,
    ) -> ::everestrs::Result<serde_json::Value> {
        match implementation_id {
            "foobar" => {
                foobar::handle_command(&self.publisher, self.foobar.as_ref(), cmd_name, parameters)
            }
            "my_store" => my_store::handle_command(
                &self.publisher,
                self.my_store.as_ref(),
                cmd_name,
                parameters,
            ),
            _ => Err(everestrs::Error::InvalidArgument(
                "Unknown implementation_id called.",
            )),
        }
    }

    #[allow(unused_variables)]
    fn handle_variable(
        &self,
        implementation_id: &str,
        name: &str,
        value: serde_json::Value,
    ) -> ::everestrs::Result<()> {
        Err(everestrs::Error::InvalidArgument(
            "Unknown variable received.",
        ))
    }

    fn on_ready(&self) {
        self.on_ready.on_ready(&self.publisher)
    }
}

mod foobar {
    use std::collections::HashMap;

    #[allow(unused_variables)]
    pub(super) fn handle_command(
        pub_impl: &super::ModulePublisher,
        foobar_service: &dyn super::ExampleServiceSubscriber,
        cmd_name: &str,
        mut parameters: HashMap<String, serde_json::Value>,
    ) -> ::everestrs::Result<serde_json::Value> {
        match cmd_name {
            "uses_something" => {
                let key: String = ::serde_json::from_value(
                    parameters
                        .remove("key")
                        .ok_or(everestrs::Error::MissingArgument("key"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("key"))?;
                let retval = foobar_service.uses_something(pub_impl, key)?;
                Ok(retval.into())
            }
            _ => Err(everestrs::Error::InvalidArgument("Unknown command called.")),
        }
    }
}

mod my_store {
    use std::collections::HashMap;

    pub(super) fn handle_command(
        pub_impl: &super::ModulePublisher,
        my_store_service: &dyn super::KvsServiceSubscriber,
        cmd_name: &str,
        mut parameters: HashMap<String, serde_json::Value>,
    ) -> ::everestrs::Result<serde_json::Value> {
        match cmd_name {
            "delete" => {
                let key: String = ::serde_json::from_value(
                    parameters
                        .remove("key")
                        .ok_or(everestrs::Error::MissingArgument("key"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("key"))?;
                let retval = my_store_service.delete(pub_impl, key)?;
                Ok(retval.into())
            }
            "exists" => {
                let key: String = ::serde_json::from_value(
                    parameters
                        .remove("key")
                        .ok_or(everestrs::Error::MissingArgument("key"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("key"))?;
                let retval = my_store_service.exists(pub_impl, key)?;
                Ok(retval.into())
            }
            "load" => {
                let key: String = ::serde_json::from_value(
                    parameters
                        .remove("key")
                        .ok_or(everestrs::Error::MissingArgument("key"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("key"))?;
                let retval = my_store_service.load(pub_impl, key)?;
                Ok(retval.into())
            }
            "store" => {
                let key: String = ::serde_json::from_value(
                    parameters
                        .remove("key")
                        .ok_or(everestrs::Error::MissingArgument("key"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("key"))?;
                let value: ::serde_json::Value = ::serde_json::from_value(
                    parameters
                        .remove("value")
                        .ok_or(everestrs::Error::MissingArgument("value"))?,
                )
                .map_err(|_| everestrs::Error::InvalidArgument("value"))?;
                let retval = my_store_service.store(pub_impl, key, value)?;
                Ok(retval.into())
            }
            _ => Err(everestrs::Error::InvalidArgument("Unknown command called.")),
        }
    }
}
