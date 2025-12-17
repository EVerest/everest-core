# Connection Test

## Standalone server

- Run `pki.sh` to build the test certificates and keys
- use `openssl s_client` to make test connections
- Run from the directory containing the test executable

### Standalone TLS server

Tests the TLS Server in isolation.

- `./connection_openssl -i <interface name>`
- gracefully terminates after 30 seconds
- requires client certificate
- Supports TCP, TLS1.2 and TLS1.3

### openssl s_client commands

TLS 1.2 and 1.3:
```sh
openssl s_client -connect [ipv6%iface]:port -verify 2 -debug -msg -verifyCAfile ./pki/certs/ca/v2g/V2G_ROOT_CA.pem -verify_return_error -cert ./pki/certs/client/vehicle/VEHICLE_LEAF.pem -cert_chain ./pki/certs/ca/vehicle/VEHICLE_CERT_CHAIN.pem -certform PEM -key ./pki/certs/client/vehicle/VEHICLE_LEAF.key -keyform PEM -pass file:./pki/certs/client/vehicle/VEHICLE_LEAF_PASSWORD.txt -ciphersuites "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256" -cipher "ECDHE-ECDSA-AES128-SHA256" -requestCAfile ./pki/certs/ca/v2g/V2G_ROOT_CA.pem
```

TLS 1.2 only:
```sh
openssl s_client -connect [ipv6%iface]:port -verify 2 -debug -msg -verifyCAfile ./pki/certs/ca/v2g/V2G_ROOT_CA.pem -verify_return_error -tls1_2 -cipher "ECDHE-ECDSA-AES128-SHA256"
```

TLS 1.3 only:
```sh
openssl s_client -connect [ipv6%iface]:port -verify 2 -debug -msg -verifyCAfile ./pki/certs/ca/v2g/V2G_ROOT_CA.pem -verify_return_error -tls1_3 -cert ./pki/certs/client/vehicle/VEHICLE_LEAF.pem -cert_chain ./pki/certs/ca/vehicle/VEHICLE_CERT_CHAIN.pem -certform PEM -key ./pki/certs/client/vehicle/VEHICLE_LEAF.key -keyform PEM -pass file:./pki/certs/client/vehicle/VEHICLE_LEAF_PASSWORD.txt -ciphersuites "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256" -requestCAfile ./pki/certs/ca/v2g/V2G_ROOT_CA.pem
```
