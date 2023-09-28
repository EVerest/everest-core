mod schema;

use argh::FromArgs;
use serde::de::DeserializeOwned;
use std::collections::HashMap;
use std::path::PathBuf;
use std::pin::Pin;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum Error {
    #[error("missing argument to command call: '{0}'")]
    MissingArgument(&'static str),
    #[error("invalid argument to command call: '{0}'")]
    InvalidArgument(&'static str),
}

pub type Result<T> = ::std::result::Result<T, Error>;

#[cxx::bridge]
mod ffi {
    struct CommandMeta {
        implementation_id: String,
        name: String,
    }

    extern "Rust" {
        type Runtime;
        fn handle_command(self: &Runtime, meta: &CommandMeta, json: JsonBlob) -> JsonBlob;
        fn on_ready(&self);
    }

    struct JsonBlob {
        data: Vec<u8>,
    }

    unsafe extern "C++" {
        include!("everestrs_sys/everestrs_sys.hpp");

        type Module;
        fn create_module(module_id: &str, prefix: &str, conf: &str) -> UniquePtr<Module>;

        /// Connects to the message broker and launches the main everest thread to push work
        /// forward. Returns the module manifest.
        fn initialize(self: Pin<&mut Module>) -> JsonBlob;

        /// Returns the interface definition.
        fn get_interface(self: &Module, interface_name: &str) -> JsonBlob;

        /// Registers the callback of the `GenericModule` to be called and calls
        /// `Everest::Module::signal_ready`.
        fn signal_ready(self: &Module, rt: &Runtime);

        /// Informs the runtime that we implement the command described in `meta` and registers the
        /// `handle_command` method from the `GenericModule` as the handler.
        fn provide_command(self: &Module, rt: &Runtime, meta: &CommandMeta);
    }
}

impl ffi::JsonBlob {
    fn as_bytes(&self) -> &[u8] {
        &self.data
    }

    fn deserialize<T: DeserializeOwned>(self) -> T {
        // TODO(hrapp): Error handling
        serde_json::from_slice(self.as_bytes()).unwrap()
    }

    fn from_vec(data: Vec<u8>) -> Self {
        Self { data }
    }
}

#[derive(FromArgs, Debug)]
/// An everest Node.
struct Args {
    /// prefix of installation.
    #[argh(option)]
    #[allow(unused)]
    pub prefix: PathBuf,

    /// configuration yml that we are running.
    #[argh(option)]
    #[allow(unused)]
    pub conf: PathBuf,

    /// module name for us.
    #[argh(option)]
    pub module: String,
}

/// Implements the handling of commands & variables, but has no specific information about the
/// details of the current module, i.e. it deals with JSON blobs and strings as command names. Code
/// generation is used to build the concrete, strongly typed abstractions that are then used by
/// final implementors.
pub trait GenericModule: Sync {
    /// Handler for the command `name` on `implementation_id` with the given `parameters`. The return value
    /// will be returned as the result of the call.
    fn handle_command(
        &self,
        implementation_id: &str,
        name: &str,
        parameters: HashMap<String, serde_json::Value>,
    ) -> Result<serde_json::Value>;

    fn on_ready(&self) {}
}

pub struct Runtime {
    // There are two subtleties here:
    // 1. We are handing out pointers to `module_impl` to `cpp_module` for callbacks. The pointers
    //    must must stay valid for as long as `cpp_module` is alive. Hence `module_impl` must never
    //    move in memory. Rust can model this through the Pin concept which upholds this guarantee.
    //    We use a Box to put the object on the heap.
    // 2. For the same reason, `module_impl` should outlive `cpp_module`, hence should be dropped
    //    after it. Rust drops fields in declaration order, hence `cpp_module` should come before
    //    `module_impl` in this struct.
    cpp_module: cxx::UniquePtr<ffi::Module>,
    module_impl: Pin<Box<dyn GenericModule>>,
}

impl Runtime {
    fn on_ready(&self) {
        self.module_impl.on_ready();
    }

    fn handle_command(&self, meta: &ffi::CommandMeta, json: ffi::JsonBlob) -> ffi::JsonBlob {
        let blob = self
            .module_impl
            .handle_command(&meta.implementation_id, &meta.name, json.deserialize())
            .unwrap();
        ffi::JsonBlob::from_vec(serde_json::to_vec(&blob).unwrap())
    }

    // TODO(hrapp): This function could use some error handling.
    pub fn from_commandline<T: GenericModule + 'static>(module_impl: T) -> Self {
        let args: Args = argh::from_env();
        let mut cpp_module = ffi::create_module(
            &args.module,
            &args.prefix.to_string_lossy(),
            &args.conf.to_string_lossy(),
        );
        let manifest_json = cpp_module.as_mut().unwrap().initialize();
        let manifest: schema::Manifest = manifest_json.deserialize();
        let module = Self {
            cpp_module,
            module_impl: Box::pin(module_impl),
        };

        // Implement all commands for all of our implementations, dispatch everything to the
        // GenericModule.
        for (implementation_id, implementation) in manifest.provides {
            let interface_s = module.cpp_module.get_interface(&implementation.interface);
            let interface: schema::Interface = interface_s.deserialize();
            for (name, _) in interface.cmds {
                let meta = ffi::CommandMeta {
                    implementation_id: implementation_id.clone(),
                    name,
                };

                module
                    .cpp_module
                    .as_ref()
                    .unwrap()
                    .provide_command(&module, &meta);
            }
        }

        // Since users can choose to overwrite `on_ready`, we can call signal_ready right away.
        // TODO(sirver): There were some doubts if this strategy is too inflexible, discuss design
        // again.
        module.cpp_module.as_ref().unwrap().signal_ready(&module);
        module
    }
}
