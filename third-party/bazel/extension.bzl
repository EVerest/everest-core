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
        url = "https://github.com/EVerest/libevse-security/archive/71870efd97bd276a21a9513baa8cc3e79b5f4665.tar.gz",
        sha256 = "f5f7e27a71084abdbd2a116baaef36dcc44ebb8d6e2826cf8ae6f891469b3d2d",
        strip_prefix = "libevse-security-71870efd97bd276a21a9513baa8cc3e79b5f4665",
        build_file = "@everest-core//third-party/bazel:BUILD.libevse-security.bazel",
    )

    maybe(
        http_archive,
        name = "libocpp",
        url = "https://github.com/EVerest/libocpp/archive/2ea181e6b24e03d7c44e96d38ec458f9a84c6085.tar.gz",
        sha256 = "cfb0a24c40319483eec1337d064959a0004bea72c56b84835df45fb0fedd1fde",
        strip_prefix = "libocpp-2ea181e6b24e03d7c44e96d38ec458f9a84c6085",
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
