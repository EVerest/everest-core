# CMake Package trailbook

This package provides CMake functions and macros to include 
the build of a trailbook documentation in a CMake-based project.

## Usage in CMake

To use this package in your CMake project, include the following line in your `CMakeLists.txt` file:

```cmake
find_package(
    trailbook
    0.1.0
    REQUIRED
    PATHS "${CMAKE_SOURCE_DIR}/<path-to-the-package>"
)
```

* Specify the version to make sure you are using 
a compatible version of the package.
* If the package is not found, CMake will 
stop with an error due to the `REQUIRED` keyword.
* If the package is not installed in a standard 
location, you can specify the path to the package using the `PATHS` option.

After finding the package, you can use the provided functions.
At the moment, the package provides the following functions:

### `add_trailbook()`

This function is the initial call for your trailbook documentation.
It can be called as follows:

```cmake
add_trailbook(
    NAME <trailbook_name>
    [STEM_DIRECTORY <stem_directory>]
    [REQUIREMENTS_TXT <requirements_txt>]
    INSTANCE_NAME <instance_name>
    [DEPLOYED_DOCS_REPO_URL <deployed_docs_repo_url>]
    [DEPLOYED_DOCS_REPO_BRANCH <deployed_docs_repo_branch>]
)
```

* This function needs to be called once per trailbook.
* The `NAME` argument specifies the name of the trailbook.
    This name will be used to create unique target names.
* The optional `STEM_DIRECTORY` argument specifies the 
    directory containing the Sphinx source files.
    If not provided, it defaults to `${CMAKE_CURRENT_SOURCE_DIR}`
* The optional `REQUIREMENTS_TXT` argument specifies the path to a 
    `requirements.txt` file for Python dependencies.
    If not provided, it defaults to `${STEM_DIRECTORY}/requirements.txt`,
    if this file exists.
    This requirements file will be used to check if the required Python packages are installed and if not to install them, if a
    python virtual environment is active
* The `INSTANCE_NAME` argument specifies the name that is used for 
    the version in the multiversion structure.
* The optional `DEPLOYED_DOCS_REPO_URL` argument specifies the URL of the
    repository where the already deployed documentation is located.
    It is required if `TRAILBOOK_<NAME>_DOWNLOAD_ALL_VERSIONS` is set to `ON`.
* The optional `DEPLOYED_DOCS_REPO_BRANCH` argument
    specifies the branch of the deployed documentation repository.
    It defaults to `main` if not provided.

## Configuring

There are several options that can be configured
for each trailbook by setting CMake variables.

### `TRAILBOOK_<NAME>_DOWNLOAD_ALL_VERSIONS`

* `<NAME>` should be replaced with the trailbook name provided
  in the `add_trailbook()` function call.

If `TRAILBOOK_<NAME>_DOWNLOAD_ALL_VERSIONS` is set to `ON`,
the build process will attempt to download all previously deployed versions
of the trailbook from the specified repository. And then embed the
new version into the multiversion structure.

If `TRAILBOOK_<NAME>_DOWNLOAD_ALL_VERSIONS` is set to `OFF` (default),
only the current version of the trailbook will be built. For this
an empty multiversion skeleton will be created.

This configuration shouldn'T be changed after the first build.

### `TRAILBOOK_<NAME>_IS_RELEASE`

* `<NAME>` should be replaced with the trailbook name provided
  in the `add_trailbook()` function call.

If `TRAILBOOK_<NAME>_IS_RELEASE` is set to `ON` (default),
the trailbook will be built as a release version. This means
that the `latest` version is updated, and the `index.html` and
`404.html` files are updated.

If `TRAILBOOK_<NAME>_IS_RELEASE` is set to `OFF`,
the mentioned files are not updated, and the `latest` version
is not changed. This can be used for example to build
nightly versions without affecting the released version.

## Building

To build the trailbook documentation, simply run the following command, after configuring the project with CMake:

```bash
cmake --build <build_directory> --target trailbook_<trailbook_name>
```

* Replace `<build_directory>` with the path to your CMake build directory.
* Replace `<trailbook_name>` with the name of your trailbook
  provided in the `add_trailbook()` function call.

This target will trigger the full build of the trailbook documentation

Furthermore, you can use the following additional targets:

```bash
cmake --build <build_directory> --target trailbook_<trailbook_name>_preview
```

This target will start a local server to preview the built documentation.

```bash
cmake --build <build_directory> --target trailbook_<trailbook_name>_live_preview
```
This target will start a local server that watches for changes
in the source files and automatically rebuilds the documentation
and refreshes the preview in the browser.

## How to build a extension for the trailbook package

The trailbook package provides a set of targets and target properties
that can be used to hook into the build process of the trailbook documentation
and extend it with custom functionality.

See the full explanation in the [EXTENDING.md](EXTENDING.md) file.
