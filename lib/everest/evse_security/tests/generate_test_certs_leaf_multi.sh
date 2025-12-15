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
  # Args:
  # $1: name of the certificate (without the .pem extension)
  # $2: directory to install the certificate and private key into
  # $3: openssl config file for the certificate
  # $4: serial number for the certificate
  # $5: CA certificate file. If this is missing, we will create a self-signed certificate.
  # $6: CA private key file. Likewise omit this to create a self-signed certificate.

  local name="$1"
  local install_dir="$2"
  local config="$3"
  local serial_num="$4"
  local signed_by_cert="$5"
  local signed_by_key="$6"

  openssl ecparam -genkey -name "$EC_CURVE" | openssl ec "$SYMMETRIC_CIPHER" -passout pass:"$password" -out "${install_dir}/${name}.key"

  if [ -z $signed_by_cert ]
  then
    openssl req -new -key "${install_dir}/${name}.key" -passin pass:"$password" -config "configs/${config}" -out "${CSR_PATH}/${name}.csr"
    openssl x509 -req -in "${CSR_PATH}/${name}.csr" -extfile "configs/${config}" -extensions ext -signkey "${install_dir}/${name}.key" -passin pass:"$password" $SHA -set_serial "${serial_num}" -out "${install_dir}/${name}.pem" -days "$VALIDITY"
  else
    openssl req -new -key "${install_dir}/${name}.key" -passin pass:"$password" -config "configs/${config}" -out "${CSR_PATH}/${name}.csr"
    openssl x509 -req -in "${CSR_PATH}/${name}.csr" -extfile "configs/${config}" -extensions ext -CA "${signed_by_cert}" -CAkey "${signed_by_key}" -passin pass:"$password" -set_serial "${serial_num}" -out "${install_dir}/${name}.pem" -days "$VALIDITY"
  fi
}

# V2G root CA
create_certificate V2G_ROOT_CA "${CA_V2G_PATH}" v2gRootCACert.cnf 12345
# Second V2G root CA
create_certificate V2G_ROOT_CA_NEW "${CA_V2G_PATH}" v2gRootCACert.cnf 12349
# Sub-CA 1
create_certificate CPO_SUB_CA1 "${CA_CSMS_PATH}" cpoSubCA1Cert.cnf 12346 "${CA_V2G_PATH}/V2G_ROOT_CA.pem" "${CA_V2G_PATH}/V2G_ROOT_CA.key"
# Sub-CA 2
create_certificate CPO_SUB_CA2 "${CA_CSMS_PATH}" cpoSubCA2Cert.cnf 12347 "${CA_CSMS_PATH}/CPO_SUB_CA1.pem" "${CA_CSMS_PATH}/CPO_SUB_CA1.key"
# Chargepoint leaf
create_certificate SECC_LEAF "${CLIENT_CSO_PATH}" seccLeafCert.cnf 12348 "${CA_CSMS_PATH}/CPO_SUB_CA2.pem" "${CA_CSMS_PATH}/CPO_SUB_CA2.key"
# Alternate chargepoint leaf
create_certificate SECC_LEAF_GRIDSYNC "${CLIENT_CSO_PATH}" seccLeafCert_Alternate.cnf 12349 "${CA_CSMS_PATH}/CPO_SUB_CA2.pem" "${CA_CSMS_PATH}/CPO_SUB_CA2.key"
# Invalid self-signed CSMS cert
create_certificate INVALID_CSMS "${CLIENT_INVALID_PATH}" v2gRootCACert.cnf 12345

# create cert chain bundles in the V2G root ca and chargepoint leaf dirs
cat "$CA_CSMS_PATH/CPO_SUB_CA2.pem" "$CA_CSMS_PATH/CPO_SUB_CA1.pem" "$CA_V2G_PATH/V2G_ROOT_CA.pem" > "$CA_V2G_PATH/V2G_CA_BUNDLE.pem"
cat "$CLIENT_CSO_PATH/SECC_LEAF.pem" "$CA_CSMS_PATH/CPO_SUB_CA2.pem" "$CA_CSMS_PATH/CPO_SUB_CA1.pem" > "$CLIENT_CSO_PATH/CPO_CERT_CHAIN.pem"

cp "$CLIENT_CSO_PATH/SECC_LEAF.key" "$CLIENT_CSMS_PATH/CSMS_LEAF.key"

# assume CSO and CSMS are same authority
cp -r $CA_CSMS_PATH/* $CA_CSO_PATH
cp "$CLIENT_CSO_PATH/SECC_LEAF.pem" "$CLIENT_CSMS_PATH/CSMS_LEAF.pem"

# empty MO bundle
touch "$CA_MO_PATH/MO_CA_BUNDLE.pem"

# Create certificates used for installation tests
create_certificate INSTALL_TEST_ROOT_CA1 "${TO_BE_INSTALLED_PATH}" install_test.cnf 21234
create_certificate INSTALL_TEST_ROOT_CA2 "${TO_BE_INSTALLED_PATH}" install_test.cnf 21235
create_certificate INSTALL_TEST_ROOT_CA3 "${TO_BE_INSTALLED_PATH}" install_test.cnf 21236
create_certificate INSTALL_TEST_ROOT_CA3_SUBCA1 "${TO_BE_INSTALLED_PATH}" install_test_subca1.cnf 21237 "${TO_BE_INSTALLED_PATH}/INSTALL_TEST_ROOT_CA3.pem" "${TO_BE_INSTALLED_PATH}/INSTALL_TEST_ROOT_CA3.key"
create_certificate INSTALL_TEST_ROOT_CA3_SUBCA2 "${TO_BE_INSTALLED_PATH}" install_test_subca2.cnf 21238 "${TO_BE_INSTALLED_PATH}/INSTALL_TEST_ROOT_CA3_SUBCA1.pem" "${TO_BE_INSTALLED_PATH}/INSTALL_TEST_ROOT_CA3_SUBCA1.key"
