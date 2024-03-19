# EvseManager documentation

see `doc.rst`

## Tests

GTest is required for building the test cases target.
To build the target and run the tests use:

```bash
cd everest-core
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./dist ..
make -j$(nproc) install
```

Tests are installed in `./modules/EvseManager/tests/`
