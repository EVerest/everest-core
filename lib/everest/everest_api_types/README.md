# EVerest API Library

This library implements the conversion between a subset of EVerest's internal types and similar external API types.
The latter are used for API modules which provide access to EVerest for external applications.

While EVerest's internal types evolve and might force changes on modules depending on them from time to time, the external API types are meant to be kept as stable as possible.

Modules acting as API servers can use this library to convert between EVerest's internal types and external API types.
The differences between EVerest's internal types and external API types have to be handled in these modules.

## Tests

All type serializations are unit tested by a round-trip conversion.
Tests are generated automatically.

## Changes to EVerest Types

### Automatic Detection

When EVerest's internal types are changed, the API conversions might be required to change as well.

To detect changes to EVerest's type definitions, the file

```
tests/expected_types_file_hashes.csv
```

stores the hashes to all types yaml files, which the library depends upon and is known to be compatible with.
When building EVerest, the actual hashes of all files listed in this file are calculated.
A unit test compares them with the stored hashes and rings an alarm bell if any of them differs.

### Type Mismatches

A hash mismatch indicates the possibility that API conversion routines require changes.
This needs manual investigation.
When changes to the API code have been implemented and the library is again compatible to the current set of types, the list of stored hashes must be updated.
The easiest way to do so is to replace `expected_types_file_hashes.csv` with the file containing the actual hashes after a build:

```
build/generated/lib/everest/everest_api_types/tests/actual_types_file_hashes.csv
```

### Extension of the API library

Whenever previously unused EVerest's internal types are used in the API library, make sure the corresponding types yaml file is listed in `expected_types_file_hashes.csv` along with its hash.
The simplest way to do so is to copy `actual_types_file_hashes.csv` after first adding the new yaml filename to `expected_types_file_hashes.csv` (with some dummy string as hash-replacement) and running a build.

## Adaption of Type changes

The general philosophy for the external API is to follow the EVerest's internal types as close as possible but at the same time to only break compatibility when unavoidable.

E.g. adding an optional field to one of EVerest's internal types should immediatly be implemented for the corresponding external API type as well.
Since API clients can ignore additional incoming fields and are free to not send optional fields themselves, compatibility is maintained.

Another example:
Adding a non-optional field to one of EVerest's internal types may in some cases still allow the external API to stay compatible.
In outgoing data (to the API client), the field's data is just not send.
For incoming data (from the API client), the field could be initialized to some default value.
Whether this is feasible must be examined on a case-by-case basis.

Other changes may unavoidably break the external API types:
If a field of a struct or a whole type is removed, then the external API types might no longer be mapped to EVerest's internal types in a meaningful way.
