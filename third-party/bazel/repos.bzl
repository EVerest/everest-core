load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@rules_rust//crate_universe:defs.bzl", "crates_repository", "crate")
load("@rules_rust//crate_universe:repositories.bzl", "crate_universe_dependencies")

def everest_core_repos():
    maybe(
        http_archive,
        name = "com_github_nelhage_rules_boost",
        url = "https://github.com/nelhage/rules_boost/archive/f02f84fac7673c56bbcfe69dea68044e6e40f92b.tar.gz",
        sha256 = "4f1f7e809960615ba8e40e3a96300acb8b4d7e6e2fb24423af7347383e4dd1bb",
        strip_prefix = "rules_boost-f02f84fac7673c56bbcfe69dea68044e6e40f92b",
    )

    maybe(
        http_archive,
        name = "everest-framework",
        url = "https://github.com/everest/everest-framework/archive/3e767e2a5652d3acb97d01fc88aae2f04f3f5282.tar.gz",
        strip_prefix = "everest-framework-3e767e2a5652d3acb97d01fc88aae2f04f3f5282",
    )
    crates_repository(
        name = "everest_core_crate_index",
        cargo_lockfile = "@everest-core//modules:Cargo.lock",
        isolated = False,
        manifests = [
            "@everest-core//modules:Cargo.toml",
            "@everest-core//modules/RsIskraMeter:Cargo.toml",
            "@everest-core//modules/RsPaymentTerminal:Cargo.toml",
            "@everest-core//modules/rust_examples/RsExample:Cargo.toml",
            "@everest-core//modules/rust_examples/RsExampleUser:Cargo.toml",
        ],
        annotations = {
            "everestrs": [crate.annotation(
                crate_features = ["build_bazel"],
            )],
        },
    )
    crate_universe_dependencies()
