def rs_everest_module(
        name,
        manifest,
        binary):
    native.genrule(
        name = "copy_to_subdir",
        srcs = [binary, manifest],
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