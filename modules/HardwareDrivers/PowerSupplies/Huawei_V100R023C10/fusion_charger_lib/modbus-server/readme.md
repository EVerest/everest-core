# Modbus Server / Client library

## Building

Build without tests:

```bash
cmake -B build
cd build
make -j$(nproc)
```

Build with tests:

```bash
cmake -B build -DBUILD_TESTING=ON
cd build
make -j$(nproc)
```

## Run examples

### Modbus basic server

Note: you need an modbus tcp client for this example

```bash
cd build
./examples/dummy_basic_server
```

### SSL Modbus server + client

#### Setup SSL certificates

```bash
cd certs
./generate.sh
```

#### Run server

```bash
cd build
./examples/dummy_basic_ssl_server
```

#### Run client

```bash
cd build
./examples/dummy_basic_ssl_client
```

## Run tests

Without coverage:

```bash
cmake -B build -DBUILD_TESTING=ON
cd build
make -j$(nproc)
ctest
```

With coverage (note that in some cases the build folder must be removed first):
```bash
cmake -B build -DBUILD_TESTING=ON -DTEST_COVERAGE=ON
cd build
make -j$(nproc)
ctest
make coverage
```