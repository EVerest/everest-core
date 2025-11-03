# Mono repo of EVerest release 2025.9.1

This branch was created by extracting code to create a single branch with
the EVerest source dependencies.

The versions were taken from the `dependencies.yaml` file.

Building is as per usual:

```
cd everest-core
cmake -S. -Bbuild -GNinja -DBUILD_TESTING=ON
ninja -C build/ -j12 -k0
```

## Next Steps

Before moving all development to a mono repo the code should be updated to the latest.

After that all development can proceed from this single branch. i.e.

1. checkout working branch
2. make updates as needed
3. create pull request

## Stand alone repositories

To facilitate working with the stand alone repositories key subdirectories
could be exported to a read-only repository.

for example:

- libocpp
- libiso15118
- libcbv2g

All future development would be in the mono repo however the stand alone
libraries would be available for use by others without needing the whole
EVerest code base.
