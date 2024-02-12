def cc_everest_module(
    name,
    srcs = [],
    deps = [],
    slots = []
):
    slots_srcs = native.glob([
        "{}/*.cpp".format(slot)
        for slot in slots
    ] + [
        "{}/*.hpp".format(slot)
        for slot in slots
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
            "@everest-utils//:ev-cli",
        ],
        cmd = """
    $(location @everest-utils//:ev-cli) module generate-loader \
        --everest-dir . \
        --schemas-dir external/everest-framework/schemas \
        --disable-clang-format \
        --output-dir `dirname $(location generated/modules/{module_name}/ld-ev.hpp)`/.. \
        {module_name}
    """.format(module_name = name)
    )


    native.cc_binary(
        name = name,
        srcs = depset(srcs + slots_srcs + module_srcs + [
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