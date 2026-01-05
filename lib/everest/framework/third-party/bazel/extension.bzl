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
        url = "https://github.com/EVerest/everest-sqlite/archive/85b31859f20255e1b96992ab35d40ebdb15d9c55.tar.gz",
        sha256 = "e1beb67c314d52036a8e65f3d00516c2f2f610264390866dedf87cf18a26bb02",
        strip_prefix = "everest-sqlite-85b31859f20255e1b96992ab35d40ebdb15d9c55",
        build_file = "@everest-framework//third-party/bazel:BUILD.everest-sqlite.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_pboettch_json-schema-validator",
        url = "https://github.com/pboettch/json-schema-validator/archive/c780404a84dd9ba978ba26bc58d17cb43fa7bc80.tar.gz",
        sha256 = "5b6ef2fd33c7fbc38fefc851f07281699fc45add5a558c2ac3f24be3e36eb0b6",
        strip_prefix = "json-schema-validator-c780404a84dd9ba978ba26bc58d17cb43fa7bc80",
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
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/85c6f7959fd40d8aaf7a50be3c9b75f08389a01c.tar.gz",
        sha256 = "eceb5b1efdaf73505ee60c1761ae457f9d663aed06009057f8fed117ed8e91b3",
        strip_prefix = "libwebsockets-85c6f7959fd40d8aaf7a50be3c9b75f08389a01c",
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
