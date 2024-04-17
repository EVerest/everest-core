load("@everest-utils//:requirements.bzl", "install_deps")
load("@rules_python//python:pip.bzl", "pip_parse")

def everest_utils_defs():
    pip_parse(
        name = "everest-testing_pip_deps",
        requirements_lock = "@everest-utils//:requirements-bazel.txt",
    )
    install_deps()
