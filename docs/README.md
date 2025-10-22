# trailbook: EVerest

The everest documentation uses the CMake package `trailbook`
to build and manage its documentation.
Additional it uses the extension package `trailbook-ext-everest`

The trailbook is initialized in `${CMAKE_SOURCE_DIR}/docs/CMakeLists.txt`
with the `add_trailbook()` function call.

At the same file an edm snapshot yaml file is added to the documentation by
calling the function `trailbook_ev_create_snapshot()`.

Module explanations, modules references, type references and interface references are added automatically in `${CMAKE_SOURCE_DIR}/cmake/everest-generate.cmake`, by calling the functions
* `trailbook_ev_generate_rst_from_manifest()`,
* `trailbook_ev_generate_rst_from_types()`,
* `trailbook_ev_generate_rst_from_interface()`, and
* `trailbook_ev_add_module_explanation()`.

There are three targets available to work with the everest documentation:

```bash
cmake --build <build_directory> --target trailbook_everest
```
Builds the everest documentation.

```bash
cmake --build <build_directory> --target trailbook_everest_preview
```
Builds the everest documentation and serves it with a local web server
for previewing.

```bash
cmake --build <build_directory> --target trailbook_everest_live_preview
```
Builds the everest documentation and serves it with a local web server
for previewing. Additionally it watches for changes in the source files
and automatically rebuilds the documentation and refreshes the preview
in the browser.
