def _everest_env(ctx):
    """Everest Root rule

    Rule creates a everest root from provided modules and config file.
    """

    # Validate the input - we make sure that the set of provided modules matches
    # the set of modules declared in config.yaml
    validation_output = ctx.actions.declare_file(ctx.attr.name + ".validation")
    ctx.actions.run(
        inputs = ctx.attr.config_file[DefaultInfo].files,
        outputs = [validation_output],
        executable = ctx.executable._validation_tool,
        arguments = [
                        "--output",
                        validation_output.path,
                        "--config",
                        ctx.attr.config_file[DefaultInfo].files.to_list()[0].path,
                        "--",
                    ] +
                    [mod.label.name for mod in ctx.attr.modules],
    )

    symlinks = {}
    files = ctx.attr.python_interpreter_target.files.to_list()
    py_interpreter = ctx.attr.python_interpreter_target[DefaultInfo].files.to_list()[0].dirname
    py_imports = []
    py_transitive_sources = []
    modules = ctx.attr.modules + ctx.attr.test_modules
    for mod in modules:
        # Python modules get a special handling.
        if PyInfo in mod:
            py_imports.extend(mod[PyInfo].imports.to_list())
            py_transitive_sources.extend(mod[PyInfo].transitive_sources.to_list())

        # Find the manifest in the data_runfiles and use its path as prefix.
        manifest = [
            file
            for file in mod[DefaultInfo].data_runfiles.files.to_list()
            if file.basename in ["manifest.yaml", "manifest.yml"]
        ][0]
        prefix = manifest.dirname

        symlinks.update(
            {
                "libexec/everest/modules/{0}{1}".format(
                    mod.label.name,
                    file.path.removeprefix(prefix),
                ): file
                for file in mod[DefaultInfo].data_runfiles.files.to_list()
                if file.path.startswith(prefix)
            },
        )
        [
            files.append(file)
            for file in mod[DefaultInfo].default_runfiles.files.to_list()
            if not file.path.startswith(prefix)
        ]

    symlinks.update({"bin/manager": ctx.attr.manager[DefaultInfo].files.to_list()[0]})
    symlinks.update(
        {
            "etc/everest/config-sil.yaml": ctx.attr.config_file[DefaultInfo].files.to_list()[0],
        },
    )
    symlinks.update(
        {
            "etc/everest/default_logging.cfg": ctx.attr.default_logging_file[DefaultInfo].files.to_list()[0],
        },
    )

    # EVerest expects that there is a `share/everest/www` directory but does
    # not care about the content... We just symlink the config.yaml into it.
    symlinks.update(
        {
            "share/everest/www/config.yaml": ctx.attr.config_file[DefaultInfo].files.to_list()[0],
        },
    )
    symlinks.update(
        {
            "share/everest/schemas/{0}".format(file.basename): file
            for file in ctx.attr.schemas[DefaultInfo].files.to_list()
        },
    )
    symlinks.update(
        {
            "share/everest/interfaces/{0}".format(file.basename): file
            for interfaces in ctx.attr.interfaces
            for file in interfaces[DefaultInfo].files.to_list()
        },
    )
    symlinks.update(
        {
            "share/everest/types/{0}".format(file.basename): file
            for types in ctx.attr.types
            for file in types[DefaultInfo].files.to_list()
        },
    )
    symlinks.update(
        {
            "share/everest/errors/{0}".format(file.basename): file
            for errors in ctx.attr.errors
            for file in errors[DefaultInfo].files.to_list()
        },
    )

    # For the executable we need to export the python specific variables by
    # hand.
    script = ctx.actions.declare_file("manager_wrapper.{}".format(ctx.label.name))
    script_content = """
#!/bin/sh
set -ex
export PATH=$(realpath {}):$PATH
declare -a PYTHON_ROOTS=({})
for i in "${{PYTHON_ROOTS[@]}}"
do
    export PYTHONPATH=$(realpath ../$i):$PYTHONPATH
done
    """.format(py_interpreter, " ".join(py_imports))
    if ctx.attr._is_test:
        script_content += """
bin/manager --prefix . --config etc/everest/config-sil.yaml --check
        """

        if ctx.attr.test_script and ctx.attr.test_script[DefaultInfo].files.to_list():
            script_content += """
bin/manager --prefix . --config etc/everest/config-sil.yaml &
PID_MANAGER=$!
{}

if ps -p $PID_MANAGER > /dev/null
then
    kill $PID_MANAGER
else
    echo "manager died"
    exit -1
fi

            """.format(ctx.attr.test_script[DefaultInfo].files.to_list()[0].path)
            files.append(ctx.attr.test_script[DefaultInfo].files.to_list()[0])
    else:
        script_content += """
bin/manager --prefix . --config etc/everest/config-sil.yaml
        """.format(py_interpreter, " ".join(py_imports))
    ctx.actions.write(script, script_content, is_executable = True)

    runfiles = ctx.runfiles(
        symlinks = symlinks,
        files = files + [script],
    )

    return [
        DefaultInfo(
            executable = script,
            runfiles = runfiles,
        ),
        OutputGroupInfo(_validation = depset([validation_output])),
        PyInfo(
            imports = depset(py_imports),
            transitive_sources = depset(py_transitive_sources),
        ),
    ]

ATTRS = {
    "config_file": attr.label(
        doc = """
The EVerest configuration file. It will be linked to
`/etc/everest/config-sil.yaml`""",
            allow_single_file = True,
        ),
    "manager": attr.label(
        doc = "The EVerest manager.",
        default = Label("@everest-framework//:manager"),
        allow_single_file = True,
        executable = True,
        cfg = "target",
    ),
    "schemas": attr.label(
        doc = "The target with the EVerest schemas.",
        default = Label("@everest-framework//schemas"),
    ),
    "interfaces": attr.label_list(
        doc = "A list of targets with EVerest interfaces.",
        default = [
            Label("@everest-framework//everestrs/tests/interfaces"),
        ],
    ),
    "types": attr.label_list(
        doc = "A list of targets with EVerest types.",
        default = [
            Label("@everest-framework//everestrs/tests/types"),
        ],
    ),
    "errors": attr.label_list(
        doc = "A list of targets with EVerest errors.",
        default = [
            Label("@everest-framework//everestrs/tests/errors"),
        ],
    ),
    "default_logging_file": attr.label(
        doc = "The target with the EVerest logging.ini file.",
        default = Label("@everest-framework//everestrs/tests:logging.ini"),
        allow_single_file = True,
    ),
    "modules": attr.label_list(
        doc = """
The list of targets with the EVerest modules under test.

The rule validates that the set of provided modules matches the set of modules
defined in the given `config_file`.""",
            allow_files = False,
        ),
    "test_modules": attr.label_list(
            doc = """
The list of targets with EVerest modules which are only enabled by the
`everest.testing` framework.

The rule will not enforce that these modules are defined in the given
`config_file`.
        """,
        allow_files = False,
    ),
    "test_script": attr.label(
        allow_single_file = True,
        doc = "A test script which will run after the manager starts.",
    ),
    "_validation_tool": attr.label(
        default = Label("@everest-framework//bazel/validate"),
        executable = True,
        cfg = "exec",
    ),
    "python_interpreter_target": attr.label(
        allow_single_file = True,
        doc = "The python interpreter to use for the EVerest environment.",
    ),
    "_is_test": attr.bool(
        default = False,
        doc = "Indicates if target is test target to validate config"
    ),
}

everest_impl_env = rule(
    implementation = _everest_env,
    attrs = ATTRS,
    executable = True,
)

everest_test = rule(
    implementation = _everest_env,
    attrs = dict(ATTRS, _is_test=attr.bool(default=True)),
    doc = """
Creates an EVerest Test.

Example:

Suppose you have the EVerest modules `ModuleFoo` and `ModuleBar` and the
EVerest config `my_config.yaml` which uses both modules. The test will launch
the modules and return when the manager process returns.

Then you can create an environment by writing:

```
everest_test(
    name = "my_everest_env",
    modules = [":ModuleFoo", ":ModuleBar"],
    config_file = ":my_config.yaml",
    python_interpreter_target=interpreter,
    test_script=":my_test_script",
)

```
Where `interpreter` is a python interpreter, for example a python3.10 resource.

You can run it with `bazel test`.
    """,
    test = True,
)

def everest_env(name, **kwargs):
    """
    Creates an EVerest environment.

    Example:

    Suppose you have the EVerest modules `ModuleFoo` and `ModuleBar` and the
    EVerest config `my_config.yaml` which uses both modules.

    Then you can create an environment by writing:

    ```
    everest_env(
        name = "my_everest_env",
        modules = [":ModuleFoo", ":ModuleBar"],
        config_file = ":my_config.yaml",
        python_interpreter_target=interpreter,
    )

    ```
    Where `interpreter` is a python interpreter, for example a python3.10 resource.

    You can either run this target with `bazel run` or pass it for example to a (py)
    test which will run your tests against the environment.
    """
    everest_impl_env(name=name, **kwargs)
    everest_test(name=name + "__manager_test", tags=["exclusive"],**kwargs)
