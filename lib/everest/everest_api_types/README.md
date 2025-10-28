# EVerest API Library

This library implements the conversion between a subset of EVerest's internal types and similar external API types.
The latter are used for API modules which provide access to EVerest for external applications.

While EVerest's internal types evolve and might force changes on modules depending on them from time to time, the external API types are meant to be kept as stable as possible.

Modules acting as API servers can use this library to convert between EVerest's internal types and external API types.
The differences between EVerest's internal types and external API types have to be handled in these modules.

## Tests

All type serializations are unit tested by a round-trip conversion.
Tests are generated automatically.

## Automatic Detection of Changes of EVerest Types and Interfaces

### Description of the Mechanism

When EVerest's internal types are changed, the API conversions might be required to change as well.
Therfore, all type files which the *everest_API_types* library depends upon and all interface files which the actual *API modules* depend upon are monitored for changes.

To detect changes to EVerest's types and interfaces definitions, the files

```bash
tests/expected_types_file_hashes.csv
tests/expected_interfaces_file_hashes.csv
```

store the hashes of type and interface yaml files.
When building EVerest, the actual hashes of all files listed in these files are calculated.
A unit test compares them with the stored hashes and fails if any of them differs.
CMake will also issue a warning if there is a mismatch, but not fail the build or the CI pipeline.

The list of monitored files must be updated manually.

```bash
$ sha256sum types/isolation_monitor.yaml
45d98b5072fa5d02a476860fe7d45a7b02f8a05eb6be94327f32dbe159d4ec40  types/isolation_monitor.yaml
```

### Type and Interface Mismatches

A hash mismatch indicates the possibility that API conversion routines require changes.
This needs manual investigation.
When changes to the API code have been implemented and the library is again compatible to the current set of types, the list of stored hashes must be updated.
The easiest way to do so is to replace `expected_types_file_hashes.csv` (or `expected_interfaces_file_hashes.csv`) with the file containing the actual hashes after a build:

```
build/generated/lib/everest/everest_api_types/tests/actual_types_file_hashes.csv
build/generated/lib/everest/everest_api_types/tests/actual_interfaces_file_hashes.csv
```

### Extension of the API library

Whenever previously unused EVerest's internal types or interfaces are used in the API library, make sure the corresponding types yaml file is listed in the correct `expected_*_file_hashes.csv` along with its hash.
The simplest way to do so is to copy the `actual_*_file_hashes.csv` after first adding the new yaml filename to `expected_*_file_hashes.csv` (with some dummy string as hash-replacement) and running a build.

## Adaption of Type Changes

### General

The general philosophy for the external API is to follow the EVerest's internal types and interfaces as close as possible but at the same time to only break compatibility when unavoidable.

E.g. adding an optional field to one of EVerest's internal types should immediatly be implemented for the corresponding external API type as well.
Since API clients can ignore additional incoming fields and are free to not send optional fields themselves, compatibility is maintained.

Another example:
Adding a non-optional field to one of EVerest's internal types may in some cases still allow the external API to stay compatible.
In outgoing data (to the API client), the field's data is just not send.
For incoming data (from the API client), the field could be initialized to some default value.
Whether this is feasible must be examined on a case-by-case basis.

Other changes may unavoidably break the external API types:
If a field of a struct or a whole type is removed, then the external API types might no longer be mappable to EVerest's internal types in a meaningful way.

### Steps to Take for Type and Interface Updates

- Add/update new/changed types in everest_api_types library
- Include changed/added type in asyncapi.yaml files
- Update/add changed/new commands and variables in asyncapi.yaml files
- If applicable: increase version number on the correct level (major, minor, patch-level)
- Check if API module code needs to be adapted to changed commands/variables
  - At the time of writing the API modules implement a one-to-one conversion between internal and external types
  - When types/interfaces change, the code might need to do real transformation of the data
- Implement new functions in the API modules to add new commands/variables