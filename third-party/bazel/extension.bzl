load("@bazel_features//:features.bzl", "bazel_features")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def _deps_impl(module_ctx):
    maybe(
        http_archive,
        name = "sigslot",
        url = "https://github.com/palacaze/sigslot/archive/33802bb3e94c4dfe07bd41b537b36806f94c3e3a.tar.gz",
        sha256 = "fe00d8a4ecc7e6b831ab4dcb3a848144b83d28494dee74596eb31d038c37f50f",
        strip_prefix = "sigslot-33802bb3e94c4dfe07bd41b537b36806f94c3e3a",
        build_file = "@everest-core//third-party/bazel:BUILD.sigslot.bazel",
    )

    maybe(
        http_archive,
        name = "pugixml",
        url = "https://github.com/zeux/pugixml/archive/dd50fa5b45ab8d58d6c27566c2eaf04a8b7e5841.tar.gz",
        sha256 = "c3ba8dddb6d8a36f374050494629c23239c6ef4868f09b934848baf2ecf44502",
        strip_prefix = "pugixml-dd50fa5b45ab8d58d6c27566c2eaf04a8b7e5841",
        build_file = "@everest-core//third-party/bazel:BUILD.pugixml.bazel",
    )

    maybe(
        http_archive,
        name = "libtimer",
        url = "https://github.com/EVerest/libtimer/archive/445ae5d7022c98603c00119506798ada85923e54.tar.gz",
        sha256 = "00c5bcb269e14b2c374082bb7de538b8d9f64756b55ff497fee412cd1c9eef1b",
        strip_prefix = "libtimer-445ae5d7022c98603c00119506798ada85923e54",
        build_file = "@everest-core//third-party/bazel:BUILD.libtimer.bazel",
    )

    maybe(
        http_archive,
        name = "libevse-security",
        url = "https://github.com/EVerest/libevse-security/archive/9f246bcca44ffe18212e919273bce281e07f3d7f.tar.gz",
        sha256 = "80cedf260cbf4282ffc04516444019e66d2f624e29949a9a56a4caf9b0883a2c",
        strip_prefix = "libevse-security-9f246bcca44ffe18212e919273bce281e07f3d7f",
        build_file = "@everest-core//third-party/bazel:BUILD.libevse-security.bazel",
    )

    maybe(
        http_archive,
        name = "libocpp",
        url = "https://github.com/EVerest/libocpp/archive/1d2ce8db1b699f13fe73c17e71d2f25f7b68da32.tar.gz",
        sha256 = "a8f1e2732c8eebc9cef75e2839a57073fd7ed6ce1b5a0d1134f98c2c030d7659",
        strip_prefix = "libocpp-1d2ce8db1b699f13fe73c17e71d2f25f7b68da32",
        build_file = "@everest-core//third-party/bazel:BUILD.libocpp.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/v4.3.3.tar.gz",
        sha256 = "6fd33527b410a37ebc91bb64ca51bdabab12b076bc99d153d7c5dd405e4bdf90",
        strip_prefix = "libwebsockets-4.3.3",
        build_file = "@everest-core//third-party/bazel:BUILD.libwebsockets.bazel",
    )

    maybe(
        http_archive,
        name = "rules_license",
        url = "https://github.com/bazelbuild/rules_license/archive/0.0.7.tar.gz",
        sha256 = "7626bea5473d3b11d44269c5b510a210f11a78bca1ed639b0f846af955b0fe31",
        strip_prefix = "rules_license-0.0.7",
    )

    maybe(
        http_archive,
        name = "rules_cc",
        url = "https://github.com/bazelbuild/rules_cc/archive/0.0.9.tar.gz",
        sha256 = "2037875b9a4456dce4a79d112a8ae885bbc4aad968e6587dca6e64f3a0900cdf",
        strip_prefix = "rules_cc-0.0.9",
    )

    maybe(
        http_archive,
        name = "platforms",
        url = "https://github.com/bazelbuild/platforms/archive/0.0.10.tar.gz",
        sha256 = "3df33228654e56b09f17613613767b052581b822d57cb9cfd5e7b19a8e617b42",
        strip_prefix = "platforms-0.0.10",
    )

    maybe(
        http_archive,
        name = "everest_sqlite",
        url = "https://github.com/EVerest/everest-sqlite/archive/v0.1.1.tar.gz",
        sha256 = "7a8a7b1edf4177d771c80a28ed0558e4b9fcb74cdda1bacdf6399766f2c8ff83",
        strip_prefix = "everest-sqlite-0.1.1",
        build_file = "@everest-core//third-party/bazel:BUILD.everest-sqlite.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_pboettch_json-schema-validator",
        url = "https://github.com/pboettch/json-schema-validator/archive/2.1.0.tar.gz",
        sha256 = "83f61d8112f485e0d3f1e72d51610ba3924b179926a8376aef3c038770faf202",
        strip_prefix = "json-schema-validator-2.1.0",
        build_file = "@everest-framework//third-party/bazel:BUILD.json-schema-validator.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_HowardHinnant_date",
        url = "https://github.com/HowardHinnant/date/archive/2ef74cb41a31dcd03b39dd5e9bf8b340669f48a4.tar.gz",
        sha256 = "3446ad7e2ba07c9105769bf6fd9b521d4e3a2f2dd0a955643a20f4adb1870efa",
        strip_prefix = "date-2ef74cb41a31dcd03b39dd5e9bf8b340669f48a4",
        build_file = "@everest-framework//third-party/bazel:BUILD.date.bazel",
    )

deps = module_extension(
    doc = "Non-module dependencies for everest-framework",
    implementation = _deps_impl,
)
