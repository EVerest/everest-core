load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")

def everest_framework_repos(repo_mapping = {}):
    maybe(
        http_archive,
        name = "com_github_nelhage_rules_boost",

        # Replace the commit hash in both places (below) with the latest, rather than using the stale one here.
        # Even better, set up Renovate and let it do the work for you (see "Suggestion: Updates" in the README).
        url = "https://github.com/nelhage/rules_boost/archive/4ab574f9a84b42b1809978114a4664184716f4bf.tar.gz",
        sha256 = "2215e6910eb763a971b1f63f53c45c0f2b7607df38c96287666d94d954da8cdc",
        strip_prefix = "rules_boost-4ab574f9a84b42b1809978114a4664184716f4bf",
        # When you first run this tool, it'll recommend a sha256 hash to put here with a message like: "DEBUG: Rule 'com_github_nelhage_rules_boost' indicated that a canonical reproducible form can be obtained by modifying arguments sha256 = ..."
    )

    maybe(
        http_archive,
        name = "rules_foreign_cc",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.5.1.tar.gz",
        sha256 = "33a5690733c5cc2ede39cb62ebf89e751f2448e27f20c8b2fbbc7d136b166804",
        strip_prefix = "rules_foreign_cc-0.5.1",
        repo_mapping = repo_mapping,
    )

    maybe(
        git_repository,
        name = "boringssl",
        commit = "b9232f9e27e5668bc0414879dcdedb2a59ea75f2",
        remote = "https://boringssl.googlesource.com/boringssl",
        shallow_since = "1637714942 +0000",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_curl_curl",
        url = "https://github.com/curl/curl/archive/refs/tags/curl-7_83_0.tar.gz",
        sha256 = "9350985ab43591ecd04b087f9c9dad84b952c35c6012e2a08ae355ae797db988",
        strip_prefix = "curl-curl-7_83_0",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.curl.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_everest_liblog",
        url = "https://github.com/EVerest/liblog/archive/5c132fb0bcdfb41d6285c4f834ae4657d7a6bff6.tar.gz",
        sha256 = "19b017a8ef4948bd0692f1d683d0031e76daa21d43a4bddff818bb77e0827fac",
        strip_prefix = "liblog-5c132fb0bcdfb41d6285c4f834ae4657d7a6bff6",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.liblog.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_pboettch_json-schema-validator",
        url = "https://github.com/pboettch/json-schema-validator/archive/f4194d7e24e2e2365660ff35b57a7c4e088b27fa.tar.gz",
        sha256 = "f71f2fbef135a61ad7bd9444f4202f9698a4b1c70279cb1e9b2567a6d996aaa1",
        strip_prefix = "json-schema-validator-f4194d7e24e2e2365660ff35b57a7c4e088b27fa",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.json-schema-validator.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_HowardHinnant_date",
        url = "https://github.com/HowardHinnant/date/archive/2ef74cb41a31dcd03b39dd5e9bf8b340669f48a4.tar.gz",
        sha256 = "3446ad7e2ba07c9105769bf6fd9b521d4e3a2f2dd0a955643a20f4adb1870efa",
        strip_prefix = "date-2ef74cb41a31dcd03b39dd5e9bf8b340669f48a4",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.date.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_rapidyaml",
        url = "https://github.com/biojppm/rapidyaml/archive/213b201d264139cd1b887790197e08850af628e3.tar.gz",
        sha256 = "c206d4565ccfa721991a8df90821d1a1f747e68385a0f3f5b9ab995e191c06be",
        strip_prefix = "rapidyaml-213b201d264139cd1b887790197e08850af628e3",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.rapidyaml.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_c4core",
        url = "https://github.com/biojppm/c4core/archive/d35c7c9bf370134595699d791e6ff8db018ddc8d.tar.gz",
        sha256 = "b768c8fb5dd4740317b7e1a3e43a0b32615d4d4e1e974d7ab515a80d2f1f318d",
        strip_prefix = "c4core-d35c7c9bf370134595699d791e6ff8db018ddc8d",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.c4core.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_biojppm_debugbreak",
        url = "https://github.com/biojppm/debugbreak/archive/5dcbe41d2bd4712c8014aa7e843723ad7b40fd74.tar.gz",
        sha256 = "4b424d23dad957937c57c142648d32571a78a6c6b2e473709748e5c1bb8a0f92",
        strip_prefix = "debugbreak-5dcbe41d2bd4712c8014aa7e843723ad7b40fd74",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.debugbreak.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_fastfloat_fastfloat",
        url = "https://github.com/fastfloat/fast_float/archive/32d21dcecb404514f94fb58660b8029a4673c2c1.tar.gz",
        sha256 = "8035a9ca28a8e3dfee332c7960af3c06ef0ab5169d5f31228b89c469e882bef7",
        strip_prefix = "fast_float-32d21dcecb404514f94fb58660b8029a4673c2c1",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.fastfloat.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_warmcatt_libwebsockets",
        url = "https://github.com/warmcat/libwebsockets/archive/b0a749c8e7a8294b68581ce4feac0e55045eb00b.tar.gz",
        sha256 = "5c3d6d482e73a0c105f3f14ce9b03c27b5d51c3938f8483b7eb8e6d535baa25f",
        strip_prefix = "libwebsockets-b0a749c8e7a8294b68581ce4feac0e55045eb00b",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.libwebsockets.bazel",
    )

    maybe(
        http_archive,
        name = "com_github_LiamBindle_mqtt-c",
        url = "https://github.com/LiamBindle/MQTT-C/archive/f69ce1e7fd54f3b1834c9c9137ce0ec5d703cb4d.tar.gz",
        sha256 = "0b3ab84e5bca3c0c29be6b84af6f9840d92a0ae4fc00ca74fdcacc30b2b0a1e9",
        strip_prefix = "MQTT-C-f69ce1e7fd54f3b1834c9c9137ce0ec5d703cb4d",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.mqtt-c.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_nlohmann_json",
        urls = ["https://github.com/nlohmann/json/archive/refs/tags/v3.11.2.tar.gz"],
        strip_prefix = "json-3.11.2",
        sha256 = "d69f9deb6a75e2580465c6c4c5111b89c4dc2fa94e3a85fcd2ffcd9a143d9273",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.nlohmann_json.bazel",
        repo_mapping = repo_mapping,
    )

    maybe(
        http_archive,
        name = "com_github_fmtlib_fmt",
        urls = ["https://github.com/fmtlib/fmt/archive/refs/tags/10.1.1.tar.gz"],
        strip_prefix = "fmt-10.1.1",
        sha256 = "78b8c0a72b1c35e4443a7e308df52498252d1cefc2b08c9a97bc9ee6cfe61f8b",
        build_file = "@com_github_everest_everest-framework//third-party/bazel:BUILD.fmt.bazel",
        repo_mapping = repo_mapping,
    )
