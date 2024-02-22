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

    binary = name + "__binary"
    manifest = native.glob(["manifest.y*ml"], allow_empty = False)[0]

    native.genrule(
        name = "ld-ev",
        outs = [
            "generated/modules/{}/ld-ev.hpp".format(name),
            "generated/modules/{}/ld-ev.cpp".format(name),
        ],
        srcs = [
            manifest,
            "@everest-core//types:types",
            "@everest-framework//schemas:schemas",
            "@everest-core//:WORKSPACE.bazel",
            "@everest-core//interfaces:interfaces",
        ],
        tools = [
            "@everest-utils//ev-dev-tools:ev-cli",
        ],
        cmd = """
    $(location @everest-utils//ev-dev-tools:ev-cli) module generate-loader \
        --work-dir `dirname $(location @everest-core//:WORKSPACE.bazel)` \
        --everest-dir ~/foo \
        --schemas-dir external/everest-framework/schemas \
        --disable-clang-format \
        --output-dir `dirname $(location generated/modules/{module_name}/ld-ev.hpp)`/.. \
        {module_name}
    """.format(module_name = name)
    )



    native.cc_binary(
        name = binary,
        srcs = depset(srcs + slots_srcs + module_srcs + [
            ":ld-ev",
        ]).to_list(),
        deps = deps + [
            "@everest-core//interfaces:interfaces_lib",
            "@everest-framework//:framework",
        ],
        copts = ["-std=c++17"],
        includes = [
            ".",
            "generated/modules/" + name,
        ],
        visibility = ["//visibility:public"],
    )

    native.genrule(
        name = "copy_to_subdir",
        srcs = [":" + binary, manifest],
        outs = [
            "{}/manifest.yaml".format(name),
            "{}/{}".format(name, name),
        ],
        cmd = "mkdir -p $(RULEDIR)/{} && ".format(name) +
              "cp $(location {}) $(RULEDIR)/{}/{} && ".format(binary, name, name) +
              "cp $(location {}) $(RULEDIR)/{}/".format(manifest, name),
    )

    native.filegroup(
        name = name,
        srcs = [
            ":copy_to_subdir",
        ],
        visibility = ["//visibility:public"],
    )
