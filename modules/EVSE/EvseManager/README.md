# EvseManager documentation

see `doc.rst`

## Tests

```bash
cd EVerest
mkdir build && cd build
cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./dist ..
make -j$(nproc) install
```

Tests are installed in `./modules/EvseManager/tests/`
