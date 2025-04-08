
# Tests

Building tests:

```sh
$ cd everest-core
$ mkdir build
$ cd build
$ cmake -GNinja -DEVEREST_CORE_BUILD_TESTING=ON ..
$ ninja install
```

`touch release.json` may be needed if it hasn't been created
(then re-run `ninja install`).

## Run EVerest in SIL

1. start MQTT broker
2. from `build/run-scripts` run `./run-sil-dc-tls.sh`
3. from `build/run-scripts` run `./nodered-sil-dc.sh`
4. open web browser [EVerest Node-RED dashboard](http://localhost:1880/ui/)

## Unit tests

- `./v2g_openssl_test`
- automatically runs `pki.sh`
- run from the directory containing the executable

### Standalone V2G TLS server

Tests the Server class via the functions in connection.cpp and
tls_connection.cpp.

- `./v2g_server -i <interface name>`
- connects to IPv6 only with a link local address
- requires `boost` library so LD_LIBRARY_PATH may need to be set
- displays the address it is listening on. e.g.
  `[fe80::ae91:a1ff:fec9:a947%3]:64109`
- supports multiple connections
- gracefully terminates after 80 seconds
- `valgrind` can be used to check memory allocations
  (has some leaks - possibly in v2g_ctx_start_events thread)
- requires client certificate
- s_client echos back what is typed with a delay since V2G has a long timeout

The connect argument must match what was displayed by `v2g_server`

```sh
openssl s_client -connect [fe80::ae91:a1ff:fec9:a947%3]:64109 -verify 2 -CAfile server_root_cert.pem -cert client_cert.pem -cert_chain client_chain.pem -key client_priv.pem -verify_return_error -verify_hostname evse.pionix.de -status
```
