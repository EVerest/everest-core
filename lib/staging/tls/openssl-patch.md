# OpenSSL 3.0.8 patch

The file `openssl-3.0.8-feat-updates-to-support-status_request_v2.patch` is a
patch to OpenSSL 3.0.8 to support the `status_request_v2` TLS extension defined
in [RFC 6961](https://datatracker.ietf.org/doc/html/rfc6961).

## Apply the patch

Assuming `openssl-3.0.8-feat-updates-to-support-status_request_v2.patch` is in
the current directory:

```sh
$ git clone --branch openssl-3.0.8 https://github.com/openssl/openssl.git
$ cd openssl
$ patch -p1 < ../openssl-3.0.8-feat-updates-to-support-status_request_v2.patch
$ ./Configure
$ make
$ sudo make install
```

The patch can also be added to `SRC_URI` in a yocto bbappend file
`openssl_3.0.8.bbappend`:

```bitbake
SRC_URI:append = " file://openssl-3.0.8-feat-updates-to-support-status_request_v2.patch"
```

## Notes

The patch is designed to be a minimal change so that `status_request_v2` can be
supported with the emphasis on TLS server support. TLS client support exists to
facilitate testing.

`status_request_v2` is deprecated for TLS 1.3 and must not be used. The code
ignores `status_request_v2` extensions when TLS 1.3 has been negotiated.

When a client requests `status_request` and `status_request_v2` then
`status_request_v2` is used and `status_request` ignored.

## Implementation

`status_request_v2` is implemented in `tls.cpp` and relies on OCSP responses
being available in separate files that are associated with the server
certificate and chain.

The patch defines `TLSEXT_STATUSTYPE_ocsp_multi` which is used in `tls.cpp` to
detect a patched version of OpenSSL.

### OpenSSL

OpenSSL contains a framework for adding handlers for TLS extensions that are not
natively handled. `status_request` is supported and the same mechanism is used
to to build the `status_request_v2` response.

Unfortunately both `status_request` and `status_request_v2` add an additional
TLS handshake record `Certificate Status` containing the OCSP responses rather
than including them as part of the extension. The OpenSSL extension framework
doesn't provide a mechanism to add a `Certificate Status` record.

The solution is to reuse the support for `status_request` and provide the
`status_request_v2` data for the `Certificate Status` record in application
code.

The patch adds the additional status type `TLSEXT_STATUSTYPE_ocsp_multi` for use
with `SSL_set_tlsext_status_type()` and updates checks on `ext.status_type` so
that it isn't rejected.

Additional functions `SSL_get_tlsext_status_expected()` and
`SSL_set_tlsext_status_expected()` are added so that application code can
indicate to OpenSSL that the `Certificate Status` record needs to be added.

`SSL_set_tlsext_status_ocsp_resp()` is used by both `status_request` and
`status_request_v2` to populate the response.

An early `Client Hello` handler is used to detect `status_request` and
`status_request_v2` extensions so that the `status_request` handler can ignore
the request (unless TLS 1.3 had been negotiated).

### OcspCache

Contains a digest method that produces a digest of a certificate. This digest
is paired with the OCSP response filename which provides the association used
in the OCSP cache.

When responding to a `status_request_v2` the server iterates through the server
certificates and builds the response including the cached OCSP response for each
certificate where available.

## Testing

The primary testing has been performed using `Wireshark` to ensure that the
`Server Hello` and `Certificate Status` records are correctly formed.

There is a googletest test suite `patched_test` that checks operation via the
OpenSSL APIs but it isn't able to check the handshake records directly.

There are a test TLS server and client that can be used to check operation.
