# trailbook: EVerest

The EVerest documentation uses the CMake package `trailbook`
to build and manage its documentation.
Additional it uses the extension package `trailbook-ext-everest`

## Configure CMake

There are three CMake variables you need to know about.

### `EVEREST_BUILD_DOCS`

The CMake variable `EVEREST_BUILD_DOCS` enables or disables the building of the
EVerest documentation. It is disabled by default. To enable it, set the variable to `ON` when configuring CMake:

```bash
cmake -D EVEREST_BUILD_DOCS=ON <path_to_source>
```

### `TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS`

The CMake variable `TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS` controls whether
all available versions of the EVerest documentation are downloaded during the build process.
By default, this variable is set to `OFF`, meaning an empty multiversion skeleton is created
during the build.

To  enable the downloading of all available versions, set the variable to `ON` when configuring CMake:

```bash
cmake -D EVEREST_BUILD_DOCS=ON -D TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS=ON <path_to_source>
```

With this the `everest.github.io` repository cloned and the new built documentation is embedded into
the exisiting multiversion structure.

### `TRAILBOOK_everest_IS_RELEASE`

If you don't want to deploy the documentation you probably don't need to care about this variable.

The CMake variable `TRAILBOOK_everest_IS_RELEASE` indicates whether the current build is a release build. It defaults to `ON` which means that files in the root of the multiversion structure
as `index.html` and `404.html` become updated. Additionally the `latest` symlink is updated to point to the current version.

In case of the need to build the documentation as nightly for example the variable can
be set to `OFF` when configuring CMake:

```bash
cmake -D EVEREST_BUILD_DOCS=ON -D TRAILBOOK_everest_DOWNLOAD_ALL_VERSIONS=ON -D TRAILBOOK_everest_IS_RELEASE=OFF <path_to_source>
```

## Build

There are three targets available to work with the EVerest documentation:

```bash
cmake --build <build_directory> --target trailbook_everest
```
Builds the EVerest documentation.

```bash
cmake --build <build_directory> --target trailbook_everest_preview
```
Builds the EVerest documentation and serves it with a local web server
for previewing.

```bash
cmake --build <build_directory> --target trailbook_everest_live_preview
```
Builds the EVerest documentation and serves it with a local web server
for previewing. Additionally it watches for changes in the source files
and automatically rebuilds the documentation and refreshes the preview
in the browser.

## How things work

The trailbook is initialized in `${CMAKE_SOURCE_DIR}/docs/CMakeLists.txt`
with the `add_trailbook()` function call.

At the same file an edm snapshot yaml file is added to the documentation by
calling the function `trailbook_ev_create_snapshot()`.

Module explanations, modules references, type references and interface references are added automatically in `${CMAKE_SOURCE_DIR}/cmake/everest-generate.cmake`, by calling the functions
* `trailbook_ev_generate_rst_from_manifest()`,
* `trailbook_ev_generate_rst_from_types()`,
* `trailbook_ev_generate_rst_from_interface()`, and
* `trailbook_ev_add_module_explanation()`.
