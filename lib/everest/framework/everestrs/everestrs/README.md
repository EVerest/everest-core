# Rust support for EVerest

This is Rust support using cxx.rs to wrap the framework C++ library.

## Trying it out

  - Install Rust as outlined on <https://rustup.rs/>, which should just be this
    one line: `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
  - Built your workspace as outlined in `everest-core` README, make sure to tell
    cMake to enable `EVEREST_ENABLE_RS_SUPPORT`.
  - You can now try building the code, but it will not do anything: `cd everestrs
    && cargo build --all`
  - You should now be able to configure the `RsExample` or `RsExampleUser` modules in your config
    YAML.

## Differences to other EVerest language wrappers

  - The `enable_external_mqtt` is ignored for Rust modules. If you want to interact
    with MQTT externally, just pull an external mqtt module (for example the
    really excellent [rumqttc](https://docs.rs/rumqttc/latest/rumqttc/)) crate
    into your module and use it directly. This is a conscious decision to future
    proof, should EVerest at some point move to something different than MQTT as
    transport layer and for cleaner abstraction.

## Status

Full support for requiring and providing interfaces is implemented, missing
currently is:

  - Support for EVerest logging
  - Support for implementations with `max_connections != 1` or `min_connections != 1`

## Mocking in Unit-Tests

The Rust wrapper supports mocking, which allows you to unit tests your modules.
To enable mocking in your code you need to do some steps however
* Add [mockall](https://github.com/asomers/mockall) and
[mockall_double](https://github.com/asomers/mockall) to your module as dependencies
* Add a `mockall` feature to your module and enable it for your tests.

Then all publishers are mocked with `mockall`.

## Building from external repositories without CMake

Note that the `everest-core` and `everest-framework` repositories are
automatically configured for this use case, this section is only relevant for
Rust modules residing in their own repositories.

While external Rust modules can be compiled using CMake without any setup,
compiling with `cargo` directly requires some additional setup. This is
required for rust-analyzer's IDE integration to work properly.

- First, build your EVerest workspace with Rust support enabled by
  passing `-DEVEREST_ENABLE_RS_SUPPORT=ON` to CMake.
- Create a file at `modules/.cargo/config.toml` in your repository.
  It file should contain the following, with `<build-dir>` replaced by the
  name of your EVerest build directory.

  ```toml
  [env.EVEREST_RS_LINK_DEPENDENCIES]
  value = "../<build-dir>/everestrs-link-dependencies.txt"
  relative = true
  ```
