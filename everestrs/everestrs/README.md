# Rust support for everest

This is Rust support using cxx.rs to wrap the framework C++ library.

## Trying it out

  - Install rust as outlined on <https://rustup.rs/>, which should just be this
    one line: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
  - Built your workspace as outlined in `everest-core` README, make sure to tell
    cMake to enable `EVEREST_ENABLE_RS_SUPPORT`. Note, that the Rust code relies
    on being built in a workspace where `make install` was run once.
  - You can now try building the code, but it will not do anything: `cd everestrs
    && cargo build --all`
  - You should now be able to configure the `RsExample` or `RsExampleUser` modules in your config
    YAML.

## Differences to other EVerest language wrappers

  - The `enable_external_mqtt` is ignored for Rust modules. If you want to interact
    with MQTT externally, just pull an external mqtt module (for example the
    really excellent [rumqttc](https://docs.rs/rumqttc/latest/rumqttc/)) crate
    into your module and use it directly. This is a concious decision to future
    proof, should everst at some point move to something different than MQTT as
    transport layer and for cleaner abstraction.

## Status

Full support for requiring and providing interfaces is implemented, missing
currently is:

  - Support for EVerest logging
  - Support for implementations with `max_connections != 1` or `min_connections != 1`
