.. _htg_debug_modules:

#####################
Debug EVerest Modules
#####################

Preparation
===========

Obvious prerequisite for using a debugger is to compile the project with
debugging enabled. One easy way to achieve this is to call

.. code-block:: bash

  cmake -B build -DCMAKE_BUILD_TYPE=Debug

from the root folder of everest-core, assuming you have already created the
``build`` directory.

Execution
=========

It is possible to use the GNU Debugger (GDB) to debug a single EVerest module.

The easiest way is to run the module in standalone mode. Say, for example, you
want to debug the Auth module for the SIL config (``config-sil.yaml``).

Let's assume you are in directory ``build/dist``.

Start the manager with

.. code-block:: bash

  ./bin/manager --config config-sil --standalone auth

This will start EVerest with the config-sil.yaml as configuration, but it
won't start the Auth module (note ``auth`` is written small because it is the
*module instance id* - this way there can be multiple Auth module instances
in your config).

Now you need to start the Auth module manual using gdb. When using
Visual Studio Code, the debug configuration (``launch.json``) looks like this:

.. code-block:: bash

  {
    "version": "0.2.0",
    "configurations": [
        {
            "name": "AuthManager",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/dist/libexec/everest/modules/Auth/Auth",
            "args": ["--config", "config-sil", "--module", "auth"],
            "stopAtEntry": false,
            "cwd": "/workspace/everest-core",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ]
        },
    ]
  }

This will then start the Auth module instance.

Also note the argument ``--module auth``, which again specifies the module
instance id and needs to match the one you've used for standalone.

Now, EVerest will continue to start and breakpoints set in the source file of
the Auth module should be taken.

.. note::

  It is also possible to debug the whole manager process. This way, you will
  have the disadvantage of possibly bad performance. The reason is that the
  manager spawns/forks new processes, which then need to be attached to the
  debugger too.

  Just in case you want to go this direction, you need to
  "set detach-on-fork off" and "follow-fork-mode" depending on what you want to
  achieve.