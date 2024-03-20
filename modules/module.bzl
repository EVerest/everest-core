# Rules for building Everest modules

def cc_everest_module(
    name,
    srcs = [],
    deps = [],
    impls = []
):
    """
    Define C++ Everest module.

    Args:
        name: Name of the module.
        srcs: List of source files, required files such as implemntations of 
            interfaces and the module main file are automatically included.
        deps: List of dependencies. Libraries that are required to build the 
            module.
        impls: List of implementations that the module has. It should match the
            content of mainifest.yaml file.
    """
    impl_srcs = native.glob([
        "{}/*.cpp".format(impl)
        for impl in impls
    ] + [
        "{}/*.hpp".format(impl)
        for impl in impls
    ])

    module_srcs = [
        name + ".cpp",
        name + ".hpp",
    ]

    native.genrule(
        name = "ld-ev",
        outs = [
            "generated/modules/{}/ld-ev.hpp".format(name),
            "generated/modules/{}/ld-ev.cpp".format(name),
        ],
        srcs = native.glob(["manifest.y*ml"], allow_empty = False) + [
            "@everest-framework//schemas:schemas",
            "//types:types",
        ],
        tools = [
            "@everest-utils//ev-dev-tools:ev-cli",
        ],
        cmd = """
    $(location @everest-utils//ev-dev-tools:ev-cli) module generate-loader \
        --everest-dir . \
        --schemas-dir external/everest-framework/schemas \
        --disable-clang-format \
        --output-dir `dirname $(location generated/modules/{module_name}/ld-ev.hpp)`/.. \
        {module_name}
    """.format(module_name = name)
    )


    native.cc_binary(
        name = name,
        srcs = depset(srcs + impl_srcs + module_srcs + [
            ":ld-ev",
        ]).to_list(),
        deps = deps + [
            "//interfaces:interfaces_lib",
            "@everest-framework//:framework",
        ],
        copts = ["-std=c++17"],
        includes = [
            ".",
            "generated/modules/" + name,
        ],
    )