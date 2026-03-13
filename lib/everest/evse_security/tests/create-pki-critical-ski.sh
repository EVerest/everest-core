#!/bin/sh
# Generates a 3-level PKI (root → CA → server) where the CA and server certs
# have the Subject Key Identifier extension marked critical (non-RFC-5280-compliant).
# Used to test the ignore_unhandled_critical_extensions workaround.

base=.
cfg=./openssl-pki-critical-ski.conf
dir=pki_critical_ski

[ ! -f "$cfg" ] && echo "missing openssl-pki-critical-ski.conf" && exit 1

mkdir -p ${base}/${dir}

root_priv=${base}/${dir}/root_priv.pem
ca_priv=${base}/${dir}/ca_priv.pem
server_priv=${base}/${dir}/server_priv.pem

root_cert=${base}/${dir}/root_cert.pem
ca_cert=${base}/${dir}/ca_cert.pem
server_cert=${base}/${dir}/server_cert.pem
cert_path=${base}/${dir}/cert_path.pem

# generate keys
for i in ${root_priv} ${ca_priv} ${server_priv}
do
    openssl genpkey -config ${cfg} -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out $i
done

export OPENSSL_CONF=${cfg}

# root cert — standard (non-critical) SKI so the trust anchor itself is clean
openssl req -config ${cfg} -x509 -section req_root -extensions v3_root \
    -key ${root_priv} -out ${root_cert}

# CA cert — critical SKI
openssl req -config ${cfg} -x509 -section req_ca -extensions v3_ca_critical_ski \
    -key ${ca_priv} -CA ${root_cert} \
    -CAkey ${root_priv} -out ${ca_cert}

# server (leaf) cert — critical SKI
openssl req -config ${cfg} -x509 -section req_server -extensions v3_server_critical_ski \
    -key ${server_priv} -CA ${ca_cert} \
    -CAkey ${ca_priv} -out ${server_cert}

# bundle: server + CA (mirrors what create-pki.sh produces)
cat ${server_cert} ${ca_cert} > ${cert_path}
