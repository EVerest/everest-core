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
        url = "https://github.com/EVerest/libtimer/archive/445ae5d7022c98603c00119506798ada85923e54.tar.gz",
        sha256 = "00c5bcb269e14b2c374082bb7de538b8d9f64756b55ff497fee412cd1c9eef1b",
        strip_prefix = "libtimer-445ae5d7022c98603c00119506798ada85923e54",
        build_file = "@everest-core//third-party/bazel:BUILD.libtimer.bazel",
    )

    maybe(
        http_archive,
        name = "libevse-security",
        url = "https://github.com/EVerest/libevse-security/archive/bf5a015cfeacad72509c1620464460753e5a5bec.tar.gz",
        sha256 = "ddf1c033d3db9c81b14240efaa0b89845d493b253d69d5f5623eebed4fe4da10",
        strip_prefix = "libevse-security-bf5a015cfeacad72509c1620464460753e5a5bec",
        build_file = "@everest-core//third-party/bazel:BUILD.libevse-security.bazel",
    )

    maybe(
        http_archive,
        name = "libocpp",
        url = "https://github.com/EVerest/libocpp/archive/157ba65d9487c149dcd0ed31179a738c4b5ca343.tar.gz",
        sha256 = "1f6ae0745399e1d1df8bfa085d689b354e96f0a8330eae5ead77408fad96c020",
        strip_prefix = "libocpp-157ba65d9487c149dcd0ed31179a738c4b5ca343",
        build_file = "@everest-core//third-party/bazel:BUILD.libocpp.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/e83ed4eb88b77039d69189327482263f6fdf5fef.tar.gz",
        sha256 = "d0a4029a2b7b557a386eb74883c1d88e15a020168c805769ab326000f2fe40fa",
        strip_prefix = "libwebsockets-e83ed4eb88b77039d69189327482263f6fdf5fef",
        build_file = "@everest-core//third-party/bazel:BUILD.libwebsockets.bazel",
    )

    maybe(
        http_archive,
        name = "everest_sqlite",
        url = "https://github.com/EVerest/everest-sqlite/archive/ccb56abb214a7f40b90598dfbec7c8d155999ada.tar.gz",
        sha256 = "9c101aa8d37020f289c26394f247f963d33f7a877fcb382449a35edb2371b540",
        strip_prefix = "everest-sqlite-ccb56abb214a7f40b90598dfbec7c8d155999ada",
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
