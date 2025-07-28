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
        url = "https://github.com/EVerest/libocpp/archive/89c7b62ec899db637f43b54f19af2c4af30cfa66.tar.gz",
        sha256 = "5f359e9b50680bee345d469b7a6e54c9bad3a308dee2bc11351860f5831dde36",
        strip_prefix = "libocpp-89c7b62ec899db637f43b54f19af2c4af30cfa66",
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
        name = "everest_sqlite",
        url = "https://github.com/EVerest/everest-sqlite/archive/v0.1.1.tar.gz",
        sha256 = "7a8a7b1edf4177d771c80a28ed0558e4b9fcb74cdda1bacdf6399766f2c8ff83",
        strip_prefix = "everest-sqlite-0.1.1",
        build_file = "@everest-core//third-party/bazel:BUILD.everest-sqlite.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_pboettch_json-schema-validator",
        url = "https://github.com/pboettch/json-schema-validator/archive/f4194d7e24e2e2365660ff35b57a7c4e088b27fa.tar.gz",
        sha256 = "f71f2fbef135a61ad7bd9444f4202f9698a4b1c70279cb1e9b2567a6d996aaa1",
        strip_prefix = "json-schema-validator-f4194d7e24e2e2365660ff35b57a7c4e088b27fa",
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
