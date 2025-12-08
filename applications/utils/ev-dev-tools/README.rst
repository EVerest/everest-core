=========================
EVerest development tools
=========================

This python project currently consists of the following packages

- `ev_cli`: EVerest module auto generation

Install
-------
To install `ev_cli`:

    python3 -m pip install .

ev_cli
------

The `ev_cli` package comes with a command line tool, named ``ev-cli``.
It has the following subcommands

- module:
  auto generation and update of EVerest modules from its interface and
  manifest definitions

- interface:
  auto generation of c++ header files for defined interfaces

- helpers:
  utility commands

There exist short forms, for all subcommands and options.  Simply call:

    ev-cli --help

for getting the list of short forms.

Both the `module` and `interface` command have the following options in
common:

- `--work-dir`:
  work directory which also contains the manifest definitions (default: ``.``)

- `--everest-dir`:
  root directory of EVerest core or any directory containing interface
  and module definitions (default: ``.``)

- `--schemas-dir`:
  schemas directory of the EVerest framework, containing the schema
  definitions (default: ``../everest-framework/schemas``)

- `--clang-format-file`:
  if c++output should be formatted, set this to the path of the
  .clang-format file

Generating c++ header files for defined interfaces
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Assuming that the interface definitions in json format are located at
``./interfaces/*.json``, simply:

    ev-cli interfaces generate-headers

This will generate the c++ header files for all interfaces and output them
to ``./generated/include/generated``.  To generate only a single interface, call:

    ev-cli interfaces generate-headers InterfaceName.json

For each interface an ``Implementation.hpp`` and ``Interface.hpp``
header file will be generated.  The former represents the `implementers`
view, and the latter the `users` view of the interface, when used in a
module.

Creating and updating auto generated files for modules (c++ only)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Assuming the modules are located at ``./modules`` and the initial
skeleton for a module named `Example` with its manifest in
``./modules/Example/manifest.json`` should be created, call:

  ev-cli module create Example

This will create the following files inside the ``./modules/Example``
subdirectory

- ``CMakeLists.txt``:
  build instruction file for CMake

- ``ld-ev.hpp``/``ld-ev.cpp``:
  glue code files for this module to get hooked up by the EVerest
  framework

- ``Example.hpp``/``Example.cpp``:
  header and source file for the module

Furthermore, for each interface provided by the module a subdirectory
with the name of the `interface id` will be created.  If, for example,
the manifest looks like::

    {
        "description": "Example module",
        "provides": {
            "main": {
                "description": "SampleInterface implementation",
                "interface": "SampleInterface",
                ...
            }
        }
        ...
    }

a subdirectory named ``main`` will be created, including two files
``SampleInterfaceImpl.hpp`` and ``SampleInterfaceImpl.cpp``.  The header
file declares the implementation of `SampleInterface`, which derives
from the auto generated interface header files from the previous
subsection.

Now it is up to the user to implement logic in the module and interface
implementation `cpp` source files.

If the modules' ``manifest.json`` or inferface definitions, used by the
module, change, you can update the generated files by using:

    ev-cli module update Example

**Note**:

1.
   ``cpp`` source files will never be changed or overwritten by the
   `update` subcommand.  The `create` subcommand only resets / overrides
   the files when using the ``--force`` option

2.
   ``hpp`` header files and the ``CMakeLists.txt`` file will get
   updated, if its interface dependencies definitions change and the
   `update` subcommand is used.  You can force an update by using the
   ``--force`` option.  During an update, the sections marked like::

        // ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
        .....
        // ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

   will be kept.  If you want to completely reset / override these
   files, you need to recreate the using `create` subcommand with the
   ``--force`` option.

3.
   Generated files will never be deleted.  So make sure, you do this if
   you, for example, change the interface ids or remove interfaces from
   the module

These additional options might be useful for the `create` and `update`
subcommands:

1. ``--force``:
   force creation or update

2. ``--diff``:
   don't touch anything, only show a `diff` of what would be changed

3. ``--only``:
   this option takes a comma separated list of files, that should be
   touched only.  This is especially helpful, if you want to recreate
   only a single interface implementation ``cpp`` file, because you
   changed the corresponding interface a lot.  To get a list of possible files, you can simply call:

    ev-cli module create Example --only which

   this would output for the above mentioned example::

        Available files for category "core"
          cmakelists
          ld-ev.hpp
          ld-ev.cpp
          module.hpp
          module.cpp
        Available files for category "interfaces"
          main.hpp
          main.cpp

   So calling:

    ev-cli module create Example --only main.cpp,cmakelists --force

   would recreate the ``CMakeLists.txt`` and the
   ``main/SampleInterfaceImpl.cpp`` files, whereas:

    ev-clie module update Example --only module.hpp

   would update only the module header file ``Example.hpp``


Auto generating NodeJS modules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**tbd**
