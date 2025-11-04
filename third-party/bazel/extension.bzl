load("@bazel_features//:features.bzl", "bazel_features")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def _deps_impl(module_ctx):
    maybe(
        http_archive,
        name = "sigslot",
        url = "https://github.com/palacaze/sigslot/archive/b588b791b9cf7eb17ff0a74d8aebd4a61166c2e1.tar.gz",
        sha256 = "140f0a2a731ed7d9ebff1bad4cb506ea2aa28fd57f42b3ec79031b1505dc6680",
        strip_prefix = "sigslot-b588b791b9cf7eb17ff0a74d8aebd4a61166c2e1",
        build_file = "@everest-core//third-party/bazel:BUILD.sigslot.bazel",
    )

    maybe(
        http_archive,
        name = "pugixml",
        url = "https://github.com/zeux/pugixml/archive/ee86beb30e4973f5feffe3ce63bfa4fbadf72f38.tar.gz",
        sha256 = "51c102d4187fac99daa38af281b0772c5e6c586f65004cdc63f8f2e011a21492",
        strip_prefix = "pugixml-ee86beb30e4973f5feffe3ce63bfa4fbadf72f38",
        build_file = "@everest-core//third-party/bazel:BUILD.pugixml.bazel",
    )

    maybe(
        http_archive,
        name = "libtimer",
        url = "https://github.com/EVerest/libtimer/archive/8b67a8eb17de70198e4e47ee8ed533c99fd25cc2.tar.gz",
        sha256 = "b37085b814e13bf65a6b9c30bf0973be44ae80f3bbcc05fa078489124ba0558a",
        strip_prefix = "libtimer-8b67a8eb17de70198e4e47ee8ed533c99fd25cc2",
        build_file = "@everest-core//third-party/bazel:BUILD.libtimer.bazel",
    )

    maybe(
        http_archive,
        name = "libevse-security",
        url = "https://github.com/EVerest/libevse-security/archive/06db4b29e0a4ea4c6c40d77b6ecae7c153f4d9d7.tar.gz",
        sha256 = "e174e75f5d4af5c08f27e12c55787239b832a2e0bf60f341d7faa066847aef3a",
        strip_prefix = "libevse-security-06db4b29e0a4ea4c6c40d77b6ecae7c153f4d9d7",
        build_file = "@everest-core//third-party/bazel:BUILD.libevse-security.bazel",
    )

    maybe(
        http_archive,
        name = "libocpp",
        url = "https://github.com/EVerest/libocpp/archive/1c6fb7c5fd92b4651648bcf3a6c8f32b58b935dc.tar.gz",
        sha256 = "f8f9794c5f73f9ab417c921d18e192b91c93f1b57bf360dbed15d6b2462e565b",
        strip_prefix = "libocpp-1c6fb7c5fd92b4651648bcf3a6c8f32b58b935dc",
        build_file = "@everest-core//third-party/bazel:BUILD.libocpp.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/adc128ca082a3c6fb9d4abbadefc09e3bc736724.tar.gz",
        sha256 = "8da42692347ba5bc3cbd89dc2b30aba3934ad51085ebeb2f8657321bf7164831",
        strip_prefix = "libwebsockets-adc128ca082a3c6fb9d4abbadefc09e3bc736724",
        build_file = "@everest-core//third-party/bazel:BUILD.libwebsockets.bazel",
    )

    maybe(
        http_archive,
        name = "everest_sqlite",
        url = "https://github.com/EVerest/everest-sqlite/archive/85b31859f20255e1b96992ab35d40ebdb15d9c55.tar.gz",
        sha256 = "e1beb67c314d52036a8e65f3d00516c2f2f610264390866dedf87cf18a26bb02",
        strip_prefix = "everest-sqlite-85b31859f20255e1b96992ab35d40ebdb15d9c55",
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
        url = "https://github.com/HowardHinnant/date/archive/f94b8f36c6180be0021876c4a397a054fe50c6f2.tar.gz",
        sha256 = "8be4c3a52d99b22a4478ce3e2a23fa4b38587ea3d3bc3d1a4d68de22c2e65fb2",
        strip_prefix = "date-f94b8f36c6180be0021876c4a397a054fe50c6f2",
        build_file = "@everest-framework//third-party/bazel:BUILD.date.bazel",
    )

deps = module_extension(
    doc = "Non-module dependencies for everest-framework",
    implementation = _deps_impl,
)
