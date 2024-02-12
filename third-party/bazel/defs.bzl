load("@com_github_nelhage_rules_boost//:boost/boost.bzl", "boost_deps")
load("@rules_python//python:repositories.bzl", "py_repositories")

def everest_core_defs():
    boost_deps()
    py_repositories()
