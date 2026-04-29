# cbv2g_json_wrapper

A native C shared library that wraps [libcbv2g](https://github.com/EVerest/libcbv2g)
with a JSON-based API for EXI encode/decode.

The wrapper is designed to replace the Java-based EXIficient codec used by the
EVerest EV-side simulation in [Josev](https://github.com/EVerest/ext-switchev-iso15118).
The intended consumer is `iso15118.shared.cbv2g_exi_codec.Cbv2gEXICodec`, which
loads `libcbv2g_json_wrapper.so` via `ctypes`.

See RFC [EVerest/ext-switchev-iso15118#58](https://github.com/EVerest/ext-switchev-iso15118/issues/58).

## Public API

```c
int cbv2g_encode(const char* json_message,
                 const char* namespace,
                 uint8_t* output_buffer,
                 size_t buffer_size,
                 size_t* output_length);

int cbv2g_decode(const uint8_t* exi_data,
                 size_t exi_length,
                 const char* namespace,
                 char* output_json,
                 size_t buffer_size);

const char* cbv2g_get_version(void);
const char* cbv2g_get_last_error(void);
void cbv2g_clear_error(void);
```

The `namespace` argument identifies the V2G schema. Supported namespaces are
listed in `include/cbv2g_json_wrapper.h`.

## Supported protocols

| Protocol | Namespace | Status |
|----------|-----------|--------|
| App Handshake (SAP) | `urn:iso:15118:2:2010:AppProtocol` | Supported |
| DIN 70121 | `urn:din:70121:2012:MsgDef` | Supported |
| ISO 15118-2 (incl. PnC) | `urn:iso:15118:2:2013:MsgDef` | Supported |
| xmldsig fragments (PnC `SignedInfo`) | `http://www.w3.org/2000/09/xmldsig#` | Supported (routed via ISO 15118-2 fragment encoder) |

ISO 15118-20 support is planned as a follow-up PR.

## Build

The wrapper is built as part of EVerest's `applications/` tree when
`-DEVEREST_BUILD_APPLICATIONS=ON` is set (the default). It links against
the in-tree `cbv2g::*` targets exported by `lib/everest/cbv2g`, so no
external dependency setup is required.

## Tests

A self-contained C test harness exercises round-trip encode/decode. Build
from the everest-core root with `-DBUILD_CBV2G_JSON_WRAPPER_TESTS=ON`:

```bash
cmake -S . -B build -DEVEREST_BUILD_APPLICATIONS=ON \
      -DBUILD_CBV2G_JSON_WRAPPER_TESTS=ON
cmake --build build --target cbv2g_test_apphand
ctest --test-dir build -R cbv2g_test_ --output-on-failure
```

## Third-party

- `third_party/cJSON/` — cJSON 1.7.19 by Dave Gamble (MIT). See its bundled
  `LICENSE` file. Used internally for JSON parsing and serialisation.
