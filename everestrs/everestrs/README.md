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
    - If you want to play with a node, check out `https://github.com/EVerest/everest-core/pull/344` in your workspace and run make install.
    - Go to `everest-core/modules/RsSomkeTest` and run `cargo build` there.
    - There is no support for building or installing Rust modules with cMake
      currently, so let's fake the installation:
    - Go to `everest-core/build/dist/libexec/everest/modules` and create the stuff needed for a module:
        ~~~bash
        mkdir RsSmokeTest
        ln -s ../../../../../../modules/RsSmokeTest/target/debug/smoke_test RsSmokeTest
        ln -s ../../../../../../modules/RsSmokeTest/manifest.yaml . 
        ~~~
    - You should now be able to configure the `RsSmokeTest` module in your config
      YAML.

## Status

This code is currently only supporting providing an interface to be implemented, i.e. no variables publish or receiving and no calling of other interfaces. Those features are straightforward, quick and easy to implement, but for now this is probably enough to iron out the integration questions.
