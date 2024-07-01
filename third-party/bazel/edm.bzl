def _edm_repositories_impl(rctx):
    rctx.file("BUILD.bazel")
    build_files_args = []
    for build_file in rctx.attr.build_files:
        build_files_args.append("--build-file")
        build_files_args.append(str(build_file))
    print("build_files_args: ", build_files_args)
    # If we don't have edm on the machine installed we try to recover and
    # install it.
    if rctx.which("edm") == None:
        print("Installind edm")
        rctx.execute(
            [
                "python3",
                "-m",
                "pip",
                "install",
                "git+https://github.com/Everest/everest-dev-environment.git#subdirectory=dependency_manager",
            ]
        )
    exec_result = rctx.execute(
        ["edm", "bazel", rctx.attr.dependencies_yaml] + build_files_args,
    )
    if exec_result.return_code != 0:
        fail("edm exec error: ", exec_result.stderr)

    rctx.file("deps.bzl", content=exec_result.stdout)


edm_repositories = repository_rule(
    _edm_repositories_impl,
    attrs={
        "dependencies_yaml": attr.label(
            mandatory=True,
            allow_single_file=True,
            doc="Path to the dependencies.yaml file",
        ),
        "build_files": attr.label_list(
            allow_files=True,
            doc="List of build files for external repositories",
        ),
    },
)
