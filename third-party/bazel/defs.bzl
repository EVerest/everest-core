load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
load("@everest_core_crate_index//:defs.bzl", "crate_repositories")

load("@rules_python//python:repositories.bzl", "py_repositories")

def everest_core_defs():
    boost_deps()
    crate_repositories()
    py_repositories()
