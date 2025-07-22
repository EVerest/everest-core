load("@bazel_features//:features.bzl", "bazel_features")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def _deps_impl(module_ctx):
    maybe(
        http_archive,
        name = "com_github_everest_liblog",
        url = "https://github.com/EVerest/liblog/archive/08ff519b647beaa51f8f25ab04b88c079ca253a7.tar.gz",
        sha256 = "32c5e419e63bffd094dcdf13adf9da7db1942029d575e7ace7559a434da967f5",
        strip_prefix = "liblog-08ff519b647beaa51f8f25ab04b88c079ca253a7",
        build_file = "@everest-framework//third-party/bazel:BUILD.liblog.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_everest_everest-sqlite",
        url = "https://github.com/EVerest/everest-sqlite/archive/ccb56abb214a7f40b90598dfbec7c8d155999ada.tar.gz",
        sha256 = "9c101aa8d37020f289c26394f247f963d33f7a877fcb382449a35edb2371b540",
        strip_prefix = "everest-sqlite-ccb56abb214a7f40b90598dfbec7c8d155999ada",
        build_file = "@everest-framework//third-party/bazel:BUILD.everest-sqlite.bazel",
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

    maybe(
        http_archive,
        name = "com_github_biojppm_rapidyaml",
        url = "https://github.com/biojppm/rapidyaml/archive/213b201d264139cd1b887790197e08850af628e3.tar.gz",
        sha256 = "c206d4565ccfa721991a8df90821d1a1f747e68385a0f3f5b9ab995e191c06be",
        strip_prefix = "rapidyaml-213b201d264139cd1b887790197e08850af628e3",
        build_file = "@everest-framework//third-party/bazel:BUILD.rapidyaml.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_c4core",
        url = "https://github.com/biojppm/c4core/archive/d35c7c9bf370134595699d791e6ff8db018ddc8d.tar.gz",
        sha256 = "b768c8fb5dd4740317b7e1a3e43a0b32615d4d4e1e974d7ab515a80d2f1f318d",
        strip_prefix = "c4core-d35c7c9bf370134595699d791e6ff8db018ddc8d",
        build_file = "@everest-framework//third-party/bazel:BUILD.c4core.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_debugbreak",
        url = "https://github.com/biojppm/debugbreak/archive/5dcbe41d2bd4712c8014aa7e843723ad7b40fd74.tar.gz",
        sha256 = "4b424d23dad957937c57c142648d32571a78a6c6b2e473709748e5c1bb8a0f92",
        strip_prefix = "debugbreak-5dcbe41d2bd4712c8014aa7e843723ad7b40fd74",
        build_file = "@everest-framework//third-party/bazel:BUILD.debugbreak.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/b0a749c8e7a8294b68581ce4feac0e55045eb00b.tar.gz",
        sha256 = "5c3d6d482e73a0c105f3f14ce9b03c27b5d51c3938f8483b7eb8e6d535baa25f",
        strip_prefix = "libwebsockets-b0a749c8e7a8294b68581ce4feac0e55045eb00b",
        build_file = "@everest-framework//third-party/bazel:BUILD.libwebsockets.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_LiamBindle_mqtt-c",
        url = "https://github.com/LiamBindle/MQTT-C/archive/f69ce1e7fd54f3b1834c9c9137ce0ec5d703cb4d.tar.gz",
        sha256 = "0b3ab84e5bca3c0c29be6b84af6f9840d92a0ae4fc00ca74fdcacc30b2b0a1e9",
        strip_prefix = "MQTT-C-f69ce1e7fd54f3b1834c9c9137ce0ec5d703cb4d",
        build_file = "@everest-framework//third-party/bazel:BUILD.mqtt-c.bazel",
    )

    maybe(
        git_repository,
        name = "libcap",
        commit = "011eb766ce43f943a4138837bdf742ac31590d26",
        remote = "https://git.kernel.org/pub/scm/libs/libcap/libcap.git",
        build_file_content = """
load("@rules_foreign_cc//foreign_cc:defs.bzl", "make")

filegroup(
    name = "all_srcs",
    srcs = glob(["**"]),
    visibility = ["//visibility:public"],
)

make(
    name = "libcap",
    lib_source = ":all_srcs",
    args = [
        "prefix=$$INSTALLDIR$$",
        "GO=false"
    ],
    out_static_libs = [
        "../lib64/libcap.a",
    ],
    visibility = [
        "//visibility:public",
    ],
)
""",    
    )

    version = "0.2.15"
    maybe(
        http_archive,
        name = "pybind11_json",
        strip_prefix = "pybind11_json-%s" % version,
        urls = ["https://github.com/pybind/pybind11_json/archive/refs/tags/%s.tar.gz" % version],
        build_file_content = """
cc_library(
    name = "pybind11_json",
    hdrs = glob(["include/**/*.hpp"]),
    visibility = [
        "//visibility:public",
    ],
    includes = ["include"]
)
""",
    )

deps = module_extension(
    doc = "Non-module dependencies for everest-framework",
    implementation = _deps_impl,
)
