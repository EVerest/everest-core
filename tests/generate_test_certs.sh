#!/bin/bash

CERT_PATH="./certs"
CSR_PATH="./csr"

EC_CURVE=prime256v1
SYMMETRIC_CIPHER=-aes-128-cbc
password="123456"

CA_CSMS_PATH="$CERT_PATH/ca/csms"
CA_CSO_PATH="$CERT_PATH/ca/cso"
CA_V2G_PATH="$CERT_PATH/ca/v2g"
CA_MO_PATH="$CERT_PATH/ca/mo"
CA_INVALID_PATH="$CERT_PATH/ca/invalid"

CLIENT_CSMS_PATH="$CERT_PATH/client/csms"
CLIENT_CSO_PATH="$CERT_PATH/client/cso"
CLIENT_V2G_PATH="$CERT_PATH/client/v2g"
CLIENT_INVALID_PATH="$CERT_PATH/client/invalid"
VALIDITY=3650

TO_BE_INSTALLED_PATH="$CERT_PATH/to_be_installed"

mkdir -p "$CERT_PATH"
mkdir -p "$CSR_PATH"
mkdir -p "$CA_CSMS_PATH"
mkdir -p "$CA_CSO_PATH"
mkdir -p "$CA_V2G_PATH"
mkdir -p "$CA_MO_PATH"
mkdir -p "$CLIENT_CSMS_PATH"
mkdir -p "$CLIENT_CSO_PATH"
mkdir -p "$CLIENT_V2G_PATH"
mkdir -p "$CLIENT_INVALID_PATH"
mkdir -p "$TO_BE_INSTALLED_PATH"


function create_certificate() {
  NAME="$1"
  SERIAL="$2"
  CONFIG="$3"
  CLIENT_PATH="$4"
  CSR_PATH="$5"
  CA_PATH="$6"

  openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "${CLIENT_PATH}/${NAME}.key"
  openssl req -new -key "$CLIENT_PATH/${NAME}.key" -passin pass:"$password" -config configs/${CONFIG} -out "${CSR_PATH}/${NAME}.csr"
  openssl x509 -req -in "$CSR_PATH/${NAME}.csr" -extfile configs/${CONFIG} -extensions ext -signkey "${CLIENT_PATH}/${NAME}.key" -passin pass:"$password" $SHA -set_serial ${SERIAL} -out "${CA_PATH}/${NAME}.pem" -days "$VALIDITY"
}

openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CLIENT_V2G_PATH/V2G_ROOT_CA.key"
openssl req -new -key "$CLIENT_V2G_PATH/V2G_ROOT_CA.key" -passin pass:"$password" -config configs/v2gRootCACert.cnf -out "$CSR_PATH/V2G_ROOT_CA.csr"
openssl x509 -req -in "$CSR_PATH/V2G_ROOT_CA.csr" -extfile configs/v2gRootCACert.cnf -extensions ext -signkey "$CLIENT_V2G_PATH/V2G_ROOT_CA.key" -passin pass:"$password" $SHA -set_serial 12345 -out "$CA_V2G_PATH/V2G_ROOT_CA.pem" -days "$VALIDITY"

openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CA_V2G_PATH/V2G_ROOT_CA_NEW.key"
openssl req -new -key "$CA_V2G_PATH/V2G_ROOT_CA_NEW.key" -passin pass:"$password" -config configs/v2gRootCACert.cnf -out "$CSR_PATH/V2G_ROOT_CA_NEW.csr"
openssl x509 -req -in "$CSR_PATH/V2G_ROOT_CA_NEW.csr" -extfile configs/v2gRootCACert.cnf -extensions ext -CA "$CA_V2G_PATH/V2G_ROOT_CA.pem" -CAkey "$CLIENT_V2G_PATH/V2G_ROOT_CA.key" -passin pass:"$password" -set_serial 12349 -out "$CA_V2G_PATH/V2G_ROOT_CA_NEW.pem" -days "$VALIDITY"

openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CLIENT_CSMS_PATH/CPO_SUB_CA1.key"
openssl req -new -key "$CLIENT_CSMS_PATH/CPO_SUB_CA1.key" -passin pass:"$password" -config configs/cpoSubCA1Cert.cnf -out "$CSR_PATH/CPO_SUB_CA1.csr"
openssl x509 -req -in "$CSR_PATH/CPO_SUB_CA1.csr" -extfile configs/cpoSubCA1Cert.cnf -extensions ext -CA "$CA_V2G_PATH/V2G_ROOT_CA.pem" -CAkey "$CLIENT_V2G_PATH/V2G_ROOT_CA.key" -passin pass:"$password" -set_serial 12346 -out "$CA_CSMS_PATH/CPO_SUB_CA1.pem" -days "$VALIDITY"

openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CLIENT_CSMS_PATH/CPO_SUB_CA2.key"
openssl req -new -key "$CLIENT_CSMS_PATH/CPO_SUB_CA2.key" -passin pass:"$password" -config configs/cpoSubCA2Cert.cnf -out "$CSR_PATH/CPO_SUB_CA2.csr"
openssl x509 -req -in "$CSR_PATH/CPO_SUB_CA2.csr" -extfile configs/cpoSubCA2Cert.cnf -extensions ext -CA "$CA_CSMS_PATH/CPO_SUB_CA1.pem" -CAkey "$CLIENT_CSMS_PATH/CPO_SUB_CA1.key" -passin pass:"$password" -set_serial 12347 -days "$VALIDITY" -out "$CA_CSMS_PATH/CPO_SUB_CA2.pem"

openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CLIENT_CSO_PATH/SECC_LEAF.key"
openssl req -new -key "$CLIENT_CSO_PATH/SECC_LEAF.key" -passin pass:"$password" -config configs/seccLeafCert.cnf -out "$CSR_PATH/SECC_LEAF.csr"
openssl x509 -req -in "$CSR_PATH/SECC_LEAF.csr" -extfile configs/seccLeafCert.cnf -extensions ext -CA "$CA_CSMS_PATH/CPO_SUB_CA2.pem" -CAkey "$CLIENT_CSMS_PATH/CPO_SUB_CA2.key" -passin pass:"$password" -set_serial 12348 -days "$VALIDITY" -out "$CLIENT_CSO_PATH/SECC_LEAF.pem"

cat "$CA_CSMS_PATH/CPO_SUB_CA2.pem" "$CA_CSMS_PATH/CPO_SUB_CA1.pem" "$CA_V2G_PATH/V2G_ROOT_CA.pem" > "$CA_V2G_PATH/V2G_CA_BUNDLE.pem"

#Invalid
openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "$CLIENT_INVALID_PATH/INVALID_CSMS.key"
openssl req -new -key "$CLIENT_INVALID_PATH/INVALID_CSMS.key" -passin pass:"$password" -config configs/v2gRootCACert.cnf -out "$CSR_PATH/INVALID_CSMS.csr"
openssl x509 -req -in "$CSR_PATH/INVALID_CSMS.csr" -extfile configs/v2gRootCACert.cnf -extensions ext -signkey "$CLIENT_INVALID_PATH/INVALID_CSMS.key" -passin pass:"$password" $SHA -set_serial 12345 -out "$CLIENT_INVALID_PATH/INVALID_CSMS.pem" -days "$VALIDITY"

cp "$CLIENT_CSO_PATH/SECC_LEAF.key" "$CLIENT_CSMS_PATH/CSMS_LEAF.key"

# assume CSO and CSMS are same authority
cp -r $CA_CSMS_PATH/* $CA_CSO_PATH
cp "$CLIENT_CSO_PATH/SECC_LEAF.pem" "$CLIENT_CSMS_PATH/CSMS_LEAF.pem"

# empty MO bundle
touch "$CA_MO_PATH/MO_CA_BUNDLE.pem"

# Certificates used for installation tests

create_certificate INSTALL_TEST_ROOT_CA1 21234 install_test.cnf "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}"
create_certificate INSTALL_TEST_ROOT_CA2 21235 install_test.cnf "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}"
create_certificate INSTALL_TEST_ROOT_CA3 21236 install_test.cnf "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}"
create_certificate INSTALL_TEST_ROOT_CA3_SUBCA1 21237 install_test_subca1.cnf "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}"
create_certificate INSTALL_TEST_ROOT_CA3_SUBCA2 21238 install_test_subca2.cnf "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}" "${TO_BE_INSTALLED_PATH}"
