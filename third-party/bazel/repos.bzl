load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def everest_core_repos():
    maybe(
        http_archive,
        name = "com_github_nelhage_rules_boost",
        url = "https://github.com/nelhage/rules_boost/archive/4ab574f9a84b42b1809978114a4664184716f4bf.tar.gz",
        sha256 = "2215e6910eb763a971b1f63f53c45c0f2b7607df38c96287666d94d954da8cdc",
        strip_prefix = "rules_boost-4ab574f9a84b42b1809978114a4664184716f4bf",
    )

    maybe(
        http_archive,
        name = "everest-framework",
        url = "https://github.com/qwello/everest-framework/archive/06983df646695303017137d00eea9f63b808fd71.tar.gz",
        strip_prefix = "everest-framework-06983df646695303017137d00eea9f63b808fd71",
    )
