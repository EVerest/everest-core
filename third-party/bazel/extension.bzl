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
        url = "https://github.com/EVerest/libevse-security/archive/fbe634af0ab3784f3cd336a309007f0f36558df6.tar.gz",
        sha256 = "8dab7dd82f8b44f9dee70435d4313963c06e9cfd1b0e38763e857da2d66b0b3d",
        strip_prefix = "libevse-security-fbe634af0ab3784f3cd336a309007f0f36558df6",
        build_file = "@everest-core//third-party/bazel:BUILD.libevse-security.bazel",
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
