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
        url = "https://github.com/HowardHinnant/date/archive/f94b8f36c6180be0021876c4a397a054fe50c6f2.tar.gz",
        sha256 = "8be4c3a52d99b22a4478ce3e2a23fa4b38587ea3d3bc3d1a4d68de22c2e65fb2",
        strip_prefix = "date-f94b8f36c6180be0021876c4a397a054fe50c6f2",
        build_file = "@everest-framework//third-party/bazel:BUILD.date.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_rapidyaml",
        url = "https://github.com/biojppm/rapidyaml/archive/47ec2fa184209687c20fd5bc05621e1cb1200311.tar.gz",
        sha256 = "4edd856d8ced361b80286cde8de40488bb268756b137add912616210a28c2744",
        strip_prefix = "rapidyaml-47ec2fa184209687c20fd5bc05621e1cb1200311",
        build_file = "@everest-framework//third-party/bazel:BUILD.rapidyaml.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_c4core",
        url = "https://github.com/biojppm/c4core/archive/7eb1f39d8c08fb98c089ec6c6ab77e7dc01c991e.tar.gz",
        sha256 = "904c1bc6dc178ebe9a581f4d7add48bf7c27310dcc9c7d3686ef4bbe0e0be577",
        strip_prefix = "c4core-7eb1f39d8c08fb98c089ec6c6ab77e7dc01c991e",
        build_file = "@everest-framework//third-party/bazel:BUILD.c4core.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_debugbreak",
        url = "https://github.com/biojppm/debugbreak/archive/328e4abca3384cbd0a69e70f263cc7b2794bff09.tar.gz",
        sha256 = "e90bc63f50e516af1a65991ffa0410fd2698fd0936041f387824b2114e72a549",
        strip_prefix = "debugbreak-328e4abca3384cbd0a69e70f263cc7b2794bff09",
        build_file = "@everest-framework//third-party/bazel:BUILD.debugbreak.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/adc128ca082a3c6fb9d4abbadefc09e3bc736724.tar.gz",
        sha256 = "8da42692347ba5bc3cbd89dc2b30aba3934ad51085ebeb2f8657321bf7164831",
        strip_prefix = "libwebsockets-adc128ca082a3c6fb9d4abbadefc09e3bc736724",
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
