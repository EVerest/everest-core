# Warnings
The Bazel support is work in progress at the moment. Depending on the exact system and
the workspace you are using it may work differently.

At the moment only the framework buid is supported, no modules are built.

The [BoringSSL](https://boringssl.googlesource.com/boringssl) is used under the hood
for encription. It may work or not work for your purposes.


# Using everst-framework from bazel workspace

To use everest-framework in the external bazel workspace, decide the git revision you 
want to depend on. 

Add the following to your WORKSPACE file, specify the slected revision in the 
`EVEREST_FRAMEWORK_REVISION` variable:

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


EVEREST_FRAMEWORK_REVISION = "..."

http_archive(
  name = "com_github_everest_everest-framework",
  url = "https://github.com/EVerest/everest-framework/archive/{}.tar.gz".format(EVEREST_FRAMEWORK_REVISION),
  strip_prefix = "everest-framework-{}".format(EVEREST_FRAMEWORK_REVISION),
)

# This load some definitions need to load dependencies on the next step
load("@com_github_everest_everest-framework//third-party/bazel:repos.bzl", "everest_framework_repos")
everest_framework_repos()

# Load all dependencies
load("@com_github_everest_everest-framework//third-party/bazel:deps.bzl", "everest_framework_deps")
everest_framework_deps()

```

After that, framework library will be available as `@com_github_everest_everest-framework//:framework` and the manager binary as `@com_github_everest_everest-framework//:manager`
