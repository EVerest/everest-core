# Utility Scripts

This directory contains useful scripts for working with everest-core and meta-everest

_cargolock2bb.py_ converts a Cargo.lock file, that can also be loaded via an URL, into a .bb file

_check_dependency_versions.py_ parses a snapshot.yaml file and checks if there are new versions of the listed dependencies available

_config2cmake.py_ parses a EVerest yaml config and prints a CMake command line to only include the modules needed by this config

_create_snapshot.py_ uses EDM to create an snapshot in a temporary subdirectory and postprocesses it to fix common problems

_parsebb.py_ parses .bb files and returns a json object containing the repository link, branch, revision and direct link to a file relative to the repo link

_replace_license.py_ parses C++ files and replaces license headers with up2date Apache 2.0 headers used in EVerest

_snapshot2bb.py_ parses a snapshot.yaml file and modifies the corresponding recipe .bb files
