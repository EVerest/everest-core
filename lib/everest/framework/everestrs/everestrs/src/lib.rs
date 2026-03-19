use everestrs_build::schema;

use clap::Parser;
use log::debug;
use serde::de::DeserializeOwned;
use std::collections::HashMap;
use std::convert::TryFrom;
use std::path::PathBuf;
use std::pin::Pin;
use std::sync::Arc;
use std::sync::Once;
use std::sync::OnceLock;
use thiserror::Error;

/// Prevent calling the init of loggers more than once.
static INIT_LOGGER_ONCE: Once = Once::new();

/// Prevent from calling [Runtime] more than once and keep it alive.
///
/// The c++ [Module] has a static lifetime. We will pass the reference of
/// the `Runtime` to the `Module` and thus the `Module` must not outlife the
/// `Runtime`. To achive this we make the lifetime of the `Runtime` also static.
static INIT_RUNTIME_ONCE: OnceLock<Pin<Arc<Runtime>>> = OnceLock::new();

// Reexport everything so the clients can use it.
pub use serde;
pub use serde_json;
pub use log;
// TODO(ddo) Drop this again - its only there as a MVP for the enum support
// of errors.
pub use serde_yaml;

/// Errors matching the exceptions defined under `exceptions.hpp`.
///
/// The tags must match the tags defined under `conversions.hpp`. For client
/// side code - always use `HandlerException`.
#[derive(Error, Debug, serde::Serialize, serde::Deserialize)]
#[serde(tag = "__everest__error_type", content = "__everest__error_msg")]
pub enum Error {
    #[error("Message Parsing Error: {0}")]
    MessageParsingError(String),

    #[error("Schema Validation Error: {0}")]
    SchemaValidationError(String),

    #[error("Handler Exception: {0}")]
    HandlerException(String),

    #[error("Command Timeout: {0}")]
    CmdTimeout(String),

    #[error("Shutdown: {0}")]
    Shutdown(String),

    #[error("Not Ready: {0}")]
    NotReady(String),
}

pub type Result<T> = ::std::result::Result<T, Error>;

#[cxx::bridge]
mod ffi {
    extern "Rust" {
        type Runtime;
        fn handle_command(
            self: &Runtime,
            implementation_id: &str,
            name: &str,
            json: JsonBlob,
        ) -> JsonBlob;
        fn handle_variable(
            self: &Runtime,
            implementation_id: &str,
            index: usize,
            name: &str,
            json: JsonBlob,
        );
        fn handle_on_error(
            self: &Runtime,
            implementation_id: &str,
            index: usize,
            error: ErrorType,
            raised: bool,
        );

        fn on_ready(&self);
    }

    struct JsonBlob {
        data: Vec<u8>,
    }

    /// The possible types a config can have. Note: Naturally this would be am
    /// enum **with** values - however, cxx can't (for now) map Rusts enums to
    /// std::variant or union.
    #[derive(Debug)]
    enum ConfigType {
        Boolean = 0,
        String = 1,
        Number = 2,
        Integer = 3,
    }

    /// One config entry: As said above, we can't use an enum and have to
    /// declare all values. Note: also Option is not an option...
    struct ConfigField {
        /// The name of the option, e.x. `max_voltage`.
        name: String,

        /// Our poor-mans enum.
        config_type: ConfigType,

        /// The value of the config field. The field has only a meaning if
        /// `conifg_type is set to [ConfigType::Boolean].
        bool_value: bool,

        /// The value of the config field. The field has only a meaning if
        /// `config_type` is set to [ConfigType::String].
        string_value: String,

        /// The value of the config field. The field has only a meaning if
        /// `config_type` is set to [ConfigType::Number].
        number_value: f64,

        /// The value of the config field. The field has only a meaning if
        /// `config_type` is set to [ConfigType::Integer].
        integer_value: i64,
    }

    /// The configs of one module. Roughly maps to the cpp's counterpart
    /// `ModuleConfig`.
    struct RsModuleConfig {
        /// The name of the group, e.x. "PowerMeter".
        module_name: String,

        /// All `ConfigFields` in this group.
        data: Vec<ConfigField>,
    }

    /// The information form the `connections` field of our current module.
    struct RsModuleConnections {
        /// The `implementation_id` of the connection block.
        implementation_id: String,

        /// Number of slots under the `implementation_id`.
        slots: usize,
    }

    #[derive(Debug)]
    pub enum ErrorSeverity {
        Low,
        Medium,
        High,
    }

    /// Rust's version of the `<utils/error.hpp>`'s Error.
    #[derive(Debug)]
    pub struct ErrorType {
        /// The type of the error. We generate that in the codegen. The
        /// full error type looks like "evse_manager/PowermeterTransactionStartFailed"
        /// and may have a namespace sprinkled into it (?).
        pub error_type: String,

        /// The description.
        pub description: String,

        /// The message - no idea what the difference to the description
        /// actually is.
        pub message: String,

        /// The severity of the error.
        pub severity: ErrorSeverity,
    }

    unsafe extern "C++" {
        include!("everestrs/src/everestrs_sys.hpp");

        type Module;
        /// Creates the module only once. The module lives then until the end
        /// of the process.
        fn create_module(
            module_id: &str,
            prefix: &str,
            mqtt_broker_socket_path: &str,
            mqtt_broker_host: &str,
            mqtt_broker_port: &u32,
            mqtt_everest_prefix: &str,
            mqtt_external_prefix: &str,
        ) -> &'static Module;

        /// Returns the manifest.
        fn get_manifest(self: &Module) -> JsonBlob;

        /// Returns the interface definition.
        fn get_interface(self: &Module, interface_name: &str) -> JsonBlob;

        /// Registers the callback of the `Subscriber` to be called and calls
        /// `Everest::Module::signal_ready`.
        fn signal_ready(self: &Module, rt: Pin<&Runtime>);

        /// Informs the runtime that we implement the command described by `implementation_id` and
        /// `name`, and registers the `handle_command` method from the `Subscriber` as the handler.
        fn provide_command(
            self: &Module,
            rt: Pin<&Runtime>,
            implementation_id: String,
            name: String,
        );

        /// Call the command described by 'implementation_id' and `name` with the given 'args'.
        /// Returns the return value.
        fn call_command(
            self: &Module,
            implementation_id: &str,
            index: usize,
            name: &str,
            args: JsonBlob,
        ) -> JsonBlob;

        /// Informs the runtime that we want to receive the variable described by
        /// `implementation_id` and `name` and registers the `handle_variable` method from the
        /// `Subscriber` as the handler.
        fn subscribe_variable(
            self: &Module,
            rt: Pin<&Runtime>,
            implementation_id: String,
            index: usize,
            name: String,
        );

        /// Subscribes to all errors of the required modules.
        fn subscribe_all_errors(self: &Module, rt: Pin<&Runtime>);

        /// Returns the `connections` block defined in the `config.yaml` for
        /// the current module.
        fn get_module_connections(self: &Module) -> Vec<RsModuleConnections>;

        /// Publishes the given `blob` under the `implementation_id` and `name`.
        fn publish_variable(self: &Module, implementation_id: &str, name: &str, blob: JsonBlob);

        /// Raises an error
        fn raise_error(self: &Module, implementation_id: &str, error: ErrorType);

        /// Clears an error
        /// If the error_type is empty, we will clear all errors from the module.
        fn clear_error(self: &Module, implementation_id: &str, error_type: &str, clear_all: bool);

        /// Returns the module config from cpp.
        fn get_module_configs(self: &Module, module_id: &str) -> Vec<RsModuleConfig>;

        /// Call this once.
        fn init_logging(module_id: &str, prefix: &str, conf: &str) -> i32;

        /// Logging sink for the EVerest module.
        fn log2cxx(level: i32, line: i32, file: &str, message: &str);
    }
}

impl ffi::JsonBlob {
    fn as_bytes(&self) -> &[u8] {
        &self.data
    }

    fn deserialize<T: DeserializeOwned>(self) -> T {
        // TODO(hrapp): Error handling
        serde_json::from_slice(self.as_bytes()).expect(&format!(
            "Failed to deserialize {:?}",
            String::from_utf8_lossy(self.as_bytes())
        ))
    }

    fn from_vec(data: Vec<u8>) -> Self {
        Self { data }
    }
}

/// Very simple logger to use by the Rust modules.
mod logger {
    use super::ffi;
    use crate::INIT_LOGGER_ONCE;

    pub(crate) struct Logger {
        level: log::Level,
    }

    impl log::Log for Logger {
        fn enabled(&self, metadata: &log::Metadata) -> bool {
            // Rust gives the Error level 1 and all other severities a higher
            // value.
            metadata.level() <= self.level
        }

        fn log(&self, record: &log::Record) {
            // The doc says `log` has to perform the filtering itself.
            if !self.enabled(record.metadata()) {
                return;
            }
            // This mapping should be kept in sync with liblog's
            // Everest::Logging::severity_level.
            let level = match record.level() {
                log::Level::Trace => 0,
                log::Level::Debug => 1,
                log::Level::Info => 2,
                log::Level::Warn => 3,
                log::Level::Error => 4,
            };

            ffi::log2cxx(
                level,
                record.line().unwrap_or_default() as i32,
                record.file().unwrap_or_default(),
                &format!("{}", record.args()),
            )
        }

        fn flush(&self) {}
    }

    impl Logger {
        /// Init the logger for everest.
        ///
        /// Don't do this on your own as we must also control some cxx code.
        pub(crate) fn init_logger(module_name: &str, prefix: &str, conf: &str) {
            INIT_LOGGER_ONCE.call_once(|| {
                let level = match ffi::init_logging(module_name, prefix, conf) {
                    0 => log::Level::Trace,
                    1 => log::Level::Debug,
                    2 => log::Level::Info,
                    3 => log::Level::Warn,
                    4 => log::Level::Error,
                    _ => log::Level::Info,
                };

                let logger = Self { level };
                log::set_boxed_logger(Box::new(logger)).unwrap();
                log::set_max_level(level.to_level_filter());
            });
        }
    }
}

/// The cpp_module is for Rust an opaque type - so Rust can't tell if it is safe
/// to be accessed from multiple threads. We know that the c++ runtime is meant
/// to be used concurrently.
unsafe impl Sync for ffi::Module {}
unsafe impl Send for ffi::Module {}

pub use ffi::{ErrorSeverity, ErrorType as FfiErrorType};

#[derive(Debug)]
pub struct ErrorType<T> {
    /// Serialised type from the FfiErrorType
    pub error_type: T,
    
    /// Carried over directly from the FfiErrorType
    pub description: String,

    /// Carried over directly from the FfiErrorType
    pub message: String,

    /// The severity of the error.
    /// Carried over directly from the FfiErrorType
    pub severity: ErrorSeverity,
}

impl<T> From<T> for ErrorType<T> {
    fn from(t: T) -> ErrorType<T> {
        ErrorType {
            error_type: t,
            description: String::new(),
            message: String::new(),
            severity: ErrorSeverity::High,
        }
    }
}

/// Arguments for an EVerest node.
#[derive(Parser, Debug)]
struct Args {
    /// prefix of installation.
    #[arg(long)]
    pub prefix: PathBuf,

    /// logging configuration that we are using.
    #[arg(long, long = "log_config")]
    pub log_config: PathBuf,

    /// module name for us.
    #[arg(long)]
    pub module: String,

    /// MQTT broker socket path
    #[arg(long = "mqtt_broker_socket_path")]
    pub mqtt_broker_socket_path: Option<PathBuf>,

    /// MQTT broker hostname
    #[arg(long = "mqtt_broker_host")]
    pub mqtt_broker_host: String,

    /// MQTT broker port
    #[arg(long = "mqtt_broker_port")]
    pub mqtt_broker_port: u32,

    /// MQTT EVerest prefix
    #[arg(long = "mqtt_everest_prefix")]
    pub mqtt_everest_prefix: String,

    /// MQTT external prefix
    #[arg(long = "mqtt_external_prefix")]
    pub mqtt_external_prefix: String,
}

/// Implements the handling of commands & variables, but has no specific information about the
/// details of the current module, i.e. it deals with JSON blobs and strings as command names. Code
/// generation is used to build the concrete, strongly typed abstractions that are then used by
/// final implementors.
pub trait Subscriber: Sync + Send {
    /// Handler for the command `name` on `implementation_id` with the given `parameters`. The return value
    /// will be returned as the result of the call.
    fn handle_command(
        &self,
        implementation_id: &str,
        name: &str,
        parameters: HashMap<String, serde_json::Value>,
    ) -> Result<serde_json::Value>;

    /// Handler for the variable `name` on `implementation_id` with the given `value`.
    fn handle_variable(
        &self,
        implementation_id: &str,
        index: usize,
        name: &str,
        value: serde_json::Value,
    ) -> Result<()>;

    /// Handler for the error raised/cleared callback
    /// The `raised` flag indicates if the error is raised or cleared.
    fn handle_on_error(
        &self,
        implementation_id: &str,
        index: usize,
        error: ffi::ErrorType,
        raised: bool,
    );

    fn on_ready(&self) {}
}

/// The [Runtime] is the central piece of the bridge between c++ and Rust. The
/// [ffi::Module] is owned by the c++ side and will outlife the [Runtime].
///
/// The `Subscriber` and the `Runtime` have a cyclic dependency. Since the
/// `Runtime` has a static lifetime we must uphold that the `Subscriber` also
/// has a static lifetime.
pub struct Runtime {
    cpp_module: &'static ffi::Module,
    sub_impl: OnceLock<Arc<dyn Subscriber>>,
}

impl Runtime {
    fn on_ready(&self) {
        self.sub_impl.get().unwrap().on_ready();
    }

    fn handle_command(&self, impl_id: &str, name: &str, json: ffi::JsonBlob) -> ffi::JsonBlob {
        debug!("handle_command: {impl_id}, {name}, '{:?}'", json.as_bytes());
        let parameters: Option<HashMap<String, serde_json::Value>> = json.deserialize();
        let retval = self
            .sub_impl
            .get()
            .unwrap()
            .handle_command(impl_id, name, parameters.unwrap_or_default());

        match retval {
            Ok(blob) => ffi::JsonBlob::from_vec(serde_json::to_vec(&blob).unwrap()),
            Err(err) => ffi::JsonBlob::from_vec(serde_json::to_vec(&err).unwrap())
        }
    }

    fn handle_variable(&self, impl_id: &str, index: usize, name: &str, json: ffi::JsonBlob) {
        debug!(
            "handle_variable: {impl_id}, {name}, '{:?}'",
            json.as_bytes()
        );
        if let Err(err) =
            self.sub_impl
                .get()
                .unwrap()
                .handle_variable(impl_id, index, name, json.deserialize())
        {
            log::error!("`handle_variable` failed: {err:?}");
        }
    }

    fn handle_on_error(&self, impl_id: &str, index: usize, error: ffi::ErrorType, raised: bool) {
        debug!("handle_on_error: {impl_id}, index {index}, raised {raised}");

        // We want to split the error type into the group and the remainder.
        self.sub_impl
            .get()
            .unwrap()
            .handle_on_error(impl_id, index, error, raised);
    }

    pub fn publish_variable<T: serde::Serialize>(
        &self,
        impl_id: &str,
        var_name: &str,
        message: &T,
    ) {
        let blob = ffi::JsonBlob::from_vec(
            serde_json::to_vec(&message).expect("Serialization of data cannot fail."),
        );
        (self.cpp_module).publish_variable(impl_id, var_name, blob);
    }

    pub fn call_command<T: serde::Serialize, R: serde::de::DeserializeOwned>(
        &self,
        impl_id: &str,
        index: usize,
        name: &str,
        args: &T,
    ) -> Result<R> {
        let blob = ffi::JsonBlob::from_vec(
            serde_json::to_vec(args).expect("Serialization of data cannot fail."),
        );
        let return_value = (self.cpp_module).call_command(impl_id, index, name, blob);
        match serde_json::from_slice(&return_value.data) {
            Ok(ok) => Ok(ok),
            Err(_) => match serde_json::from_slice::<Error>(&return_value.data) {
                Ok(err) => Err(err),
                Err(err) => Err(Error::MessageParsingError(format!("{err:?}")))
            }
        }
    }

    /// Called from the generated code.
    /// The type T should be an error.
    pub fn raise_error<T: serde::Serialize + core::fmt::Debug>(&self, impl_id: &str, error: ErrorType<T>) {
        let error_string = serde_yaml::to_string(&error.error_type).unwrap_or_default();
        // Remove the new line -> this should be gone once we stop using yaml
        // since we don't really want yaml.
        let error_string = error_string.strip_suffix("\n").unwrap_or(&error_string);

        debug!("Raising error {error_string:?} from {error:?}");
        let error_type = ffi::ErrorType {
            error_type: error_string.to_string(),
            description: error.description,
            message: error.message,
            severity: error.severity,
        };
        self.cpp_module.raise_error(impl_id, error_type);
    }

    /// Called from the generated code.
    /// The type T should be an error.
    pub fn clear_error<T: serde::Serialize + core::fmt::Debug>(
        &self,
        impl_id: &str,
        error: T,
        clear_all: bool,
    ) {
        let error_string = serde_yaml::to_string(&error).unwrap_or_default();
        let mut error_string = error_string.strip_suffix("\n").unwrap_or(&error_string);

        // The yaml conversion changes empty strings into a string containing two
        // single quotes which we catch and convert to an actual empty string
        if error_string == "''" {
            error_string = "";
        }

        debug!("Clearing the {error_string} from {error:?}");
        self.cpp_module
            .clear_error(impl_id, &error_string, clear_all);
    }

    // TODO(hrapp): This function could use some error handling.
    pub fn new() -> Pin<Arc<Self>> {
        INIT_RUNTIME_ONCE
            .get_or_init(|| {
                let args: Args = Args::parse();
                logger::Logger::init_logger(
                    &args.module,
                    &args.prefix.to_string_lossy(),
                    &args.log_config.to_string_lossy(),
                );

                let cpp_module = ffi::create_module(
                    &args.module,
                    &args.prefix.to_string_lossy(),
                    &args
                        .mqtt_broker_socket_path
                        .unwrap_or_default()
                        .to_string_lossy(),
                    &args.mqtt_broker_host,
                    &args.mqtt_broker_port,
                    &args.mqtt_everest_prefix,
                    &args.mqtt_external_prefix,
                );
                Arc::pin(Self {
                    cpp_module,
                    sub_impl: OnceLock::new(),
                })
            })
            .clone()
    }

    pub fn set_subscriber(self: Pin<&Self>, sub_impl: Arc<dyn Subscriber>) {
        self.sub_impl
            .set(sub_impl)
            .unwrap_or_else(|_| panic!("set_subscriber called twice"));
        let manifest_json = self.cpp_module.get_manifest();
        let manifest: schema::Manifest = manifest_json.deserialize();
        log::debug!("Deserialiazed the manifest {manifest:?}");

        // Implement all commands for all of our implementations, dispatch everything to the
        // Subscriber.
        for (implementation_id, provides) in manifest.provides {
            let interface_s = self.cpp_module.get_interface(&provides.interface);
            let interface: schema::InterfaceFromEverest = interface_s.deserialize();
            log::debug!("Deserialiazed the interface {interface:?}");

            for (name, _) in interface.cmds {
                self.cpp_module
                    .provide_command(self, implementation_id.clone(), name);
            }
        }

        let connections = self.get_module_connections();

        // Subscribe to all variables that might be of interest.
        for (implementation_id, requires) in manifest.requires {
            let connection = connections.get(&implementation_id).cloned().unwrap_or(0);
            let interface_s = self.cpp_module.get_interface(&requires.interface);
            // EVerest framework may return null if an interface is not used in
            // the config (the connection is then 0).
            if interface_s.as_bytes() == b"null" && connection == 0 {
                debug!("Skipping the interface {implementation_id}");
                continue;
            }
            let interface: schema::InterfaceFromEverest = interface_s.deserialize();
            log::debug!("Deserialiazed the interface {interface:?}");

            for i in 0usize..connection {
                for (name, _) in interface.vars.iter() {
                    if requires.ignore.vars.contains(name) {
                        continue;
                    }
                    self.cpp_module.subscribe_variable(
                        self,
                        implementation_id.clone(),
                        i,
                        name.clone(),
                    );
                }
            }
        }

        self.cpp_module.subscribe_all_errors(self);

        // Since users can choose to overwrite `on_ready`, we can call signal_ready right away.
        // TODO(hrapp): There were some doubts if this strategy is too inflexible, discuss design
        // again.
        (self.cpp_module).signal_ready(self);
    }

    /// The interface for fetching the module connections though the C++ runtime.
    pub fn get_module_connections(&self) -> HashMap<String, usize> {
        let raw_connections = self.cpp_module.get_module_connections();
        raw_connections
            .into_iter()
            .map(|connection| (connection.implementation_id, connection.slots))
            .collect()
    }
}

/// A store for our config values. The type is closely related to
/// [ffi::ConfigField] and [ffi::ConfigType].
#[derive(Debug)]
pub enum Config {
    Boolean(bool),
    String(String),
    Number(f64),
    Integer(i64),
}

impl TryFrom<&Config> for bool {
    type Error = Error;
    fn try_from(value: &Config) -> std::result::Result<Self, Self::Error> {
        match value {
            Config::Boolean(value) => Ok(*value),
            _ => Err(Error::MessageParsingError(format!("{:?}", value))),
        }
    }
}

impl TryFrom<&Config> for String {
    type Error = Error;
    fn try_from(value: &Config) -> std::result::Result<Self, Self::Error> {
        match value {
            Config::String(value) => Ok(value.clone()),
            _ => Err(Error::MessageParsingError(format!("{:?}", value))),
        }
    }
}

impl TryFrom<&Config> for f64 {
    type Error = Error;
    fn try_from(value: &Config) -> std::result::Result<Self, Self::Error> {
        match value {
            Config::Number(value) => Ok(*value),
            _ => Err(Error::MessageParsingError(format!("{:?}", value))),
        }
    }
}

impl TryFrom<&Config> for i64 {
    type Error = Error;
    fn try_from(value: &Config) -> std::result::Result<Self, Self::Error> {
        match value {
            Config::Integer(value) => Ok(*value),
            _ => Err(Error::MessageParsingError(format!("{:?}", value))),
        }
    }
}

/// Interface for fetching the configurations through the C++ runtime.
///
/// This is separetated from the module since the user might need the config
/// to create the [Runtime].
pub fn get_module_configs() -> HashMap<String, HashMap<String, Config>> {
    let args: Args = Args::parse();
    logger::Logger::init_logger(
        &args.module,
        &args.prefix.to_string_lossy(),
        &args.log_config.to_string_lossy(),
    );
    let cpp_module = ffi::create_module(
        &args.module,
        &args.prefix.to_string_lossy(),
        &args
            .mqtt_broker_socket_path
            .unwrap_or_default()
            .to_string_lossy(),
        &args.mqtt_broker_host,
        &args.mqtt_broker_port,
        &args.mqtt_everest_prefix,
        &args.mqtt_external_prefix,
    );
    let raw_config = cpp_module.get_module_configs(&args.module);

    // Convert the nested Vec's into nested HashMaps.
    let mut out: HashMap<String, HashMap<String, Config>> = HashMap::new();
    for mm_config in raw_config {
        let cc_config = mm_config
            .data
            .into_iter()
            .map(|field| {
                let value = match field.config_type {
                    ffi::ConfigType::Boolean => Config::Boolean(field.bool_value),
                    ffi::ConfigType::String => Config::String(field.string_value),
                    ffi::ConfigType::Number => Config::Number(field.number_value),
                    ffi::ConfigType::Integer => Config::Integer(field.integer_value),
                    _ => panic!("Unexpected value {:?}", field.config_type),
                };

                (field.name, value)
            })
            .collect::<HashMap<_, _>>();

        // If we have already an entry with the `module_name`, we try to extend
        // it.
        out.entry(mm_config.module_name)
            .or_default()
            .extend(cc_config);
    }

    out
}
