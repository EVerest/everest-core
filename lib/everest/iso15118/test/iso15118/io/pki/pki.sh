#!/bin/sh
VALIDITY_OEM_LEAF_CERT=1460
VALIDITY_OEM_SUBCA2_CERT=1460
VALIDITY_OEM_SUBCA1_CERT=1460
VALIDITY_OEM_ROOT_CERT=3650
VALIDITY_SECC_LEAF_CERT=60
VALIDITY_CPO_SUBCA1_CERT=1460
VALIDITY_CPO_SUBCA2_CERT=365
VALIDITY_V2G_ROOT_CERT=3650
VALIDITY_VEHICLE_LEAF_CERT=1460
VALIDITY_VEHICLE_SUBCA1_CERT=1460
VALIDITY_VEHICLE_SUBCA2_CERT=3650

SYMMETRIC_CIPHER=-aes-128-cbc  # TODO Check correct version for ISO 15118-20
SYMMETRIC_CIPHER_PKCS12=-aes128  # TODO Check correct version for ISO 15118-20
SHA=-sha256  # TODO Check correct version for ISO 15118-20
EC_CURVE=prime256v1  # TODO Check correct version for ISO 15118-20

password=123456

echo "Password used is: '$password'"

# 0) Create directories if not yet existing
CERT_PATH=certs
CSR_PATH=csrs

CA_CSO_PATH=$CERT_PATH/ca/cso
CA_OEM_PATH=$CERT_PATH/ca/oem
CA_V2G_PATH=$CERT_PATH/ca/v2g
CA_VEHICLE_PATH=$CERT_PATH/ca/vehicle

CLIENT_CSO_PATH=$CERT_PATH/client/cso
CLIENT_OEM_PATH=$CERT_PATH/client/oem
CLIENT_V2G_PATH=$CERT_PATH/client/v2g
CLIENT_VEHICLE_PATH=$CERT_PATH/client/vehicle

mkdir -p $CERT_PATH
mkdir -p $CSR_PATH
mkdir -p $CA_CSO_PATH
mkdir -p $CA_OEM_PATH
mkdir -p $CA_V2G_PATH
mkdir -p $CA_VEHICLE_PATH
mkdir -p $CLIENT_CSO_PATH
mkdir -p $CLIENT_OEM_PATH
mkdir -p $CLIENT_V2G_PATH
mkdir -p $CLIENT_VEHICLE_PATH

# 1) Create a self-signed V2G_ROOT_CA certificate
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_V2G_PATH/V2G_ROOT_CA.key
openssl req -new -key $CLIENT_V2G_PATH/V2G_ROOT_CA.key -passin pass:$password -config configs/v2gRootCACert.cnf -out $CSR_PATH/V2G_ROOT_CA.csr
openssl x509 -req -in $CSR_PATH/V2G_ROOT_CA.csr -extfile configs/v2gRootCACert.cnf -extensions ext -signkey $CLIENT_V2G_PATH/V2G_ROOT_CA.key -passin pass:$password $SHA -set_serial 12345 -out $CA_V2G_PATH/V2G_ROOT_CA.pem -days $VALIDITY_V2G_ROOT_CERT

# 2) Create an intermediate CPO sub-CA 1 certificate which is directly signed
#    by the V2G_ROOT_CA certificate
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_CSO_PATH/CPO_SUB_CA1.key
openssl req -new -key $CLIENT_CSO_PATH/CPO_SUB_CA1.key -passin pass:$password -config configs/cpoSubCA1Cert.cnf -out $CSR_PATH/CPO_SUB_CA1.csr
openssl x509 -req -in $CSR_PATH/CPO_SUB_CA1.csr -extfile configs/cpoSubCA1Cert.cnf -extensions ext -CA $CA_V2G_PATH/V2G_ROOT_CA.pem -CAkey $CLIENT_V2G_PATH/V2G_ROOT_CA.key -passin pass:$password -set_serial 12346 -out $CA_CSO_PATH/CPO_SUB_CA1.pem -days $VALIDITY_CPO_SUBCA1_CERT

# 3) Create a second intermediate CPO sub-CA certificate (sub-CA 2) just the way
#    the previous intermedia certificate was created, which is directly signed
#    by the CPO_SUB_CA1
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_CSO_PATH/CPO_SUB_CA2.key
openssl req -new -key $CLIENT_CSO_PATH/CPO_SUB_CA2.key -passin pass:$password -config configs/cpoSubCA2Cert.cnf -out $CSR_PATH/CPO_SUB_CA2.csr
openssl x509 -req -in $CSR_PATH/CPO_SUB_CA2.csr -extfile configs/cpoSubCA2Cert.cnf -extensions ext -CA $CA_CSO_PATH/CPO_SUB_CA1.pem -CAkey $CLIENT_CSO_PATH/CPO_SUB_CA1.key -passin pass:$password -set_serial 12347 -days $VALIDITY_CPO_SUBCA2_CERT -out $CA_CSO_PATH/CPO_SUB_CA2.pem

# 4) Create an SECC certificate, which is the leaf certificate belonging to
#    the charging station that authenticates itself to the EVCC during a TLS
#    handshake, signed by CPO_SUB_CA2
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_CSO_PATH/SECC_LEAF.key
openssl req -new -key $CLIENT_CSO_PATH/SECC_LEAF.key -passin pass:$password -config configs/seccLeafCert.cnf -out $CSR_PATH/SECC_LEAF.csr
openssl x509 -req -in $CSR_PATH/SECC_LEAF.csr -extfile configs/seccLeafCert.cnf -extensions ext -CA $CA_CSO_PATH/CPO_SUB_CA2.pem -CAkey $CLIENT_CSO_PATH/CPO_SUB_CA2.key -passin pass:$password -set_serial 12348 -days $VALIDITY_SECC_LEAF_CERT -out $CLIENT_CSO_PATH/SECC_LEAF.pem
cat $CLIENT_CSO_PATH/SECC_LEAF.pem $CA_CSO_PATH/CPO_SUB_CA2.pem $CA_CSO_PATH/CPO_SUB_CA1.pem > $CLIENT_CSO_PATH/CPO_CERT_CHAIN.pem

# 5) Create a self-signed OEM_ROOT_CA certificate (validity is up to the OEM)
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_OEM_PATH/OEM_ROOT_CA.key
openssl req -new -key $CLIENT_OEM_PATH/OEM_ROOT_CA.key -passin pass:$password -config configs/oemRootCACert.cnf -out $CSR_PATH/OEM_ROOT_CA.csr
openssl x509 -req -in $CSR_PATH/OEM_ROOT_CA.csr -extfile configs/oemRootCACert.cnf -extensions ext -signkey $CLIENT_OEM_PATH/OEM_ROOT_CA.key -passin pass:$password $SHA -set_serial 12349 -out $CA_OEM_PATH/OEM_ROOT_CA.pem -days $VALIDITY_OEM_ROOT_CERT

# 6) Create an intermediate OEM sub-CA certificate, which is directly signed by
#    the OEM_ROOT_CA certificate (validity is up to the OEM)
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_OEM_PATH/OEM_SUB_CA1.key
openssl req -new -key $CLIENT_OEM_PATH/OEM_SUB_CA1.key -passin pass:$password -config configs/oemSubCA1Cert.cnf -out $CSR_PATH/OEM_SUB_CA1.csr
openssl x509 -req -in $CSR_PATH/OEM_SUB_CA1.csr -extfile configs/oemSubCA1Cert.cnf -extensions ext -CA $CA_OEM_PATH/OEM_ROOT_CA.pem -CAkey $CLIENT_OEM_PATH/OEM_ROOT_CA.key -passin pass:$password -set_serial 12350 -days $VALIDITY_OEM_SUBCA1_CERT -out $CA_OEM_PATH/OEM_SUB_CA1.pem

# 7) Create a second intermediate OEM sub-CA certificate, which is directly
#    signed by the OEM_SUB_CA1 certificate (validity is up to the OEM)
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_OEM_PATH/OEM_SUB_CA2.key
openssl req -new -key $CLIENT_OEM_PATH/OEM_SUB_CA2.key -passin pass:$password -config configs/oemSubCA2Cert.cnf -out $CSR_PATH/OEM_SUB_CA2.csr
openssl x509 -req -in $CSR_PATH/OEM_SUB_CA2.csr -extfile configs/oemSubCA2Cert.cnf -extensions ext -CA $CA_OEM_PATH/OEM_SUB_CA1.pem -CAkey $CLIENT_OEM_PATH/OEM_SUB_CA1.key -passin pass:$password -set_serial 12351 -days $VALIDITY_OEM_SUBCA2_CERT -out $CA_OEM_PATH/OEM_SUB_CA2.pem

# 8) Create an OEM provisioning certificate, which is the leaf certificate
#    belonging to the OEM certificate chain (used for contract certificate
#    installation)
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_OEM_PATH/OEM_LEAF.key
openssl req -new -key $CLIENT_OEM_PATH/OEM_LEAF.key -passin pass:$password -config configs/oemLeafCert.cnf -out $CSR_PATH/OEM_LEAF.csr
openssl x509 -req -in $CSR_PATH/OEM_LEAF.csr -extfile configs/oemLeafCert.cnf -extensions ext -CA $CA_OEM_PATH/OEM_SUB_CA2.pem -CAkey $CLIENT_OEM_PATH/OEM_SUB_CA2.key -passin pass:$password -set_serial 12352 -days $VALIDITY_OEM_LEAF_CERT -out $CLIENT_OEM_PATH/OEM_LEAF.pem
cat $CLIENT_OEM_PATH/OEM_LEAF.pem $CA_OEM_PATH/OEM_SUB_CA2.pem $CA_OEM_PATH/OEM_SUB_CA1.pem > $CA_OEM_PATH/OEM_CERT_CHAIN.pem

# 16) Create an intermediate vehicle sub-CA 1 certificate which is directly signed
#    by the V2GRootCA certificate
# ---------------------------------------------------------------------------
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA1.key
openssl req -new -key $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA1.key -passin pass:$password -config configs/vehicleSubCA1Cert.cnf -out $CSR_PATH/VEHICLE_SUB_CA1.csr
openssl x509 -req -in $CSR_PATH/VEHICLE_SUB_CA1.csr -extfile configs/vehicleSubCA1Cert.cnf -extensions ext -CA $CA_V2G_PATH/V2G_ROOT_CA.pem -CAkey $CLIENT_V2G_PATH/V2G_ROOT_CA.key -passin pass:$password -set_serial 12360 -out $CA_VEHICLE_PATH/VEHICLE_SUB_CA1.pem -days $VALIDITY_VEHICLE_SUBCA1_CERT

# 17) Create a second intermediate vehicle sub-CA certificate (sub-CA 2) just the way
#    the previous intermedia certificate was created, which is directly signed
#    by the VEHICLE_SUB_CA1
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA2.key
openssl req -new -key $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA2.key -passin pass:$password -config configs/vehicleSubCA2Cert.cnf -out $CSR_PATH/VEHICLE_SUB_CA2.csr
openssl x509 -req -in $CSR_PATH/VEHICLE_SUB_CA2.csr -extfile configs/vehicleSubCA2Cert.cnf -extensions ext -CA $CA_VEHICLE_PATH/VEHICLE_SUB_CA1.pem -CAkey $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA1.key -passin pass:$password -set_serial 12361 -days $VALIDITY_VEHICLE_SUBCA2_CERT -out $CA_VEHICLE_PATH/VEHICLE_SUB_CA2.pem

# 18) Create an vehicle certificate, which is the leaf certificate belonging to
#    the electric vehicle that authenticates itself to the SECC during a TLS
#    handshake, signed by vehicleSubCA2
openssl ecparam -genkey -name $EC_CURVE | openssl ec $SYMMETRIC_CIPHER -passout pass:$password -out $CLIENT_VEHICLE_PATH/VEHICLE_LEAF.key
openssl req -new -key $CLIENT_VEHICLE_PATH/VEHICLE_LEAF.key -passin pass:$password -config configs/vehicleLeafCert.cnf -out $CSR_PATH/VEHICLE_LEAF.csr
openssl x509 -req -in $CSR_PATH/VEHICLE_LEAF.csr -extfile configs/vehicleLeafCert.cnf -extensions ext -CA $CA_VEHICLE_PATH/VEHICLE_SUB_CA2.pem -CAkey $CLIENT_VEHICLE_PATH/VEHICLE_SUB_CA2.key -passin pass:$password -set_serial 12362 -days $VALIDITY_VEHICLE_LEAF_CERT -out $CLIENT_VEHICLE_PATH/VEHICLE_LEAF.pem
cat $CLIENT_VEHICLE_PATH/VEHICLE_LEAF.pem $CA_VEHICLE_PATH/VEHICLE_SUB_CA2.pem $CA_VEHICLE_PATH/VEHICLE_SUB_CA1.pem > $CA_VEHICLE_PATH/VEHICLE_CERT_CHAIN.pem

# 19) Place all passwords to generated private keys in separate text files.
#     In this script, even though we use a single password for all certificates,
#     certificates from a different source could have been generated with a different
#     passphrase/passkey/password altogether. Leave them empty if no password is required.
echo $password > $CLIENT_CSO_PATH/SECC_LEAF_PASSWORD.txt
echo $password > $CLIENT_OEM_PATH/OEM_LEAF_PASSWORD.txt
echo $password > $CLIENT_V2G_PATH/V2G_ROOT_CA_PASSWORD.txt
echo $password > $CLIENT_VEHICLE_PATH/VEHICLE_LEAF_PASSWORD.txt
