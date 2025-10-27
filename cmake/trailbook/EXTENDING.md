# Extending the trailbook package

The trailbook package provides a set of targets and target properties
that can be used to hook into the build process of the trailbook documentation
and extend it with custom functionality.

## Important Note

Since the trailbook packages work a lot with custom CMake targets and
custom CMake commands, it is important to set dependencies correctly
when extending the trailbook package.

This means that it is not sufficient to just depend on the targets
and extend target dependencies with `add_dependencies()`. Instead,
you should also make sure to extend the file-level dependencies. For this
a set of custom target properties is provided that can be used
to add additional dependencies to the custom commands used in the
trailbook build process.

## Available Stages to Hook Into

To hook into the build process custom commands can be placed in between
stages

### Hook in before stage: Prepare Sphinx Source

If you want to hook into the build process before the Sphinx source
is prepared, you can define a custom command that doesn't need to
depend on any files, but the created files and targets should be appended to
the target list property `ADDITIONAL_DEPS_STAGE_PREPARE_SPHINX_SOURCE_BEFORE`.

This can be done by using the following code snippet:

```cmake
# Your custom cmake code here
add_custom_command(
    OUTPUT
        <output_file1>
        <output_file2>
    COMMAND
        <your_command_here>
    DEPENDS
        <your_dependencies_here>
)

add_custom_target(
    <wrapper_target_name>
    DEPENDS
        <output_file1>
        <output_file2>
)

# Hook into the trailbook build process
set_property(
    TARGET trailbook_<trailbook_name>
    APPEND
    PROPERTY 
        ADDITIONAL_DEPS_STAGE_PREPARE_SPHINX_SOURCE_BEFORE
            <wrapper_target_name>
            <output_file1>
            <output_file2>
)
```

* `<output_file1>`, `<output_file2>` can be any custom files created
    by your command.
* `<wrapper_target_name>` is a custom target that 
    wraps your command for example.
* `<trailbook_name>` should be replaced with the name of your trailbook
  provided in the `add_trailbook()` function call.

With this target-level dependencies and file-level dependencies can be added.
If there is a target that depends on the output files, the file-level
dependencies should be added as well.

### Hook in before stage: Build Sphinx

If you want to hook in before the Sphinx build process starts,
you can use the target list property `ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE`.
and `DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER` to add file-level dependencies
to the stage before.

This can be done by using the following code snippet:

```cmake
# Hook into the trailbook build process after the prepare stage
get_target_property(
    DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
    trailbook_<trailbook_name>
    DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER
)

# Your custom cmake code here
add_custom_command(
    OUTPUT
        <output_file1>
        <output_file2>
    DEPENDS
        <your_dependencies_here>
        ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
    COMMAND
        <your_command_here>
)
add_custom_target(
    <wrapper_target_name>
    DEPENDS
        <output_file1>
        <output_file2>
        ${DEPS_STAGE_PREPARE_SPHINX_SOURCE_AFTER}
)

# Hook into the trailbook build process before the build stage
set_property(
    TARGET trailbook_<trailbook_name>
    APPEND
    PROPERTY
        ADDITIONAL_DEPS_STAGE_BUILD_SPHINX_BEFORE
            <wrapper_target_name>
            <output_file1>
            <output_file2>
)
```

* `<output_file1>`, `<output_file2>` can be any custom files created
    by your command.
* `<wrapper_target_name>` is a custom target that
    wraps your command for example.
* `<trailbook_name>` should be replaced with the name of your trailbook
    provided in the `add_trailbook()` function call.

With the `get_target_property()` call the file-level dependencies
from the previous stage are retrieved and added to the custom command
and the custom target. This ensures that the custom command is executed
after the previous stage is completed.

With the `set_property()` call the custom target and the output files
are added to the target-level dependencies of the build stage.
This ensures that the build stage waits for the custom command
to complete before starting the Sphinx build process.

### Hook in before stage: Post Process Sphinx

This can be done analogously to the previous stage, but using the target list property
`ADDITIONAL_DEPS_STAGE_POSTPROCESS_SPHINX_BEFORE` and `DEPS_STAGE_BUILD_SPHINX_AFTER`.

```cmake
# Hook into the trailbook build process after the build stage
get_target_property(
    DEPS_STAGE_BUILD_SPHINX_AFTER
    trailbook_<trailbook_name>
    DEPS_STAGE_BUILD_SPHINX_AFTER
)
# Your custom cmake code here
add_custom_command(
    OUTPUT
        <output_file1>
        <output_file2>
    DEPENDS
        <your_dependencies_here>
        ${DEPS_STAGE_BUILD_SPHINX_AFTER}
    COMMAND
        <your_command_here>
)
add_custom_target(
    <wrapper_target_name>
    DEPENDS
        <output_file1>
        <output_file2>
        ${DEPS_STAGE_BUILD_SPHINX_AFTER}
)
# Hook into the trailbook build process before the post process stage
set_property(
    TARGET trailbook_<trailbook_name>
    APPEND
    PROPERTY
        ADDITIONAL_DEPS_STAGE_POSTPROCESS_SPHINX_BEFORE
            <wrapper_target_name>
            <output_file1>
            <output_file2>
)
```
