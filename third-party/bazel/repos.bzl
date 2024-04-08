load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

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
        url = "https://github.com/everest/everest-framework/archive/b9f9b5ddff46856027346a98f5c16d11b16f7436.tar.gz",
        strip_prefix = "everest-framework-b9f9b5ddff46856027346a98f5c16d11b16f7436",
    )
