#!/bin/sh

cfg=openssl-pki.conf

server_root_priv=server_root_priv.pem
server_ca_priv=server_ca_priv.pem
server_priv=server_priv.pem

server_root_cert=server_root_cert.pem
server_ca_cert=server_ca_cert.pem
server_cert=server_cert.pem
server_chain=server_chain.pem

client_root_priv=client_root_priv.pem
client_ca_priv=client_ca_priv.pem
client_priv=client_priv.pem

client_root_cert=client_root_cert.pem
client_ca_cert=client_ca_cert.pem
client_cert=client_cert.pem
client_chain=client_chain.pem

# generate keys
for i in    ${server_root_priv} ${server_ca_priv} ${server_priv} \
            ${client_root_priv} ${client_ca_priv} ${client_priv}
do
    openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out $i
    chmod 644 $i
done

export OPENSSL_CONF=${cfg}

echo "Generate server root"
openssl req \
    -config ${cfg} -x509 -section req_server_root -extensions v3_server_root \
    -key ${server_root_priv} -out ${server_root_cert}
echo "Generate server ca"
openssl req \
    -config ${cfg} -x509 -section req_server_ca -extensions v3_server_ca \
    -key ${server_ca_priv} -CA ${server_root_cert} -CAkey ${server_root_priv} -out ${server_ca_cert}
echo "Generate server"
openssl req \
    -config ${cfg} -x509 -section req_server -extensions v3_server \
    -key ${server_priv} -CA ${server_ca_cert} -CAkey ${server_ca_priv} -out ${server_cert}
cat ${server_cert} ${server_ca_cert} > ${server_chain}

echo "Generate client root"
openssl req \
    -config ${cfg} -x509 -section req_client_root -extensions v3_client_root \
    -key ${client_root_priv} -out ${client_root_cert}
echo "Generate client ca"
openssl req \
    -config ${cfg} -x509 -section req_client_ca -extensions v3_client_ca \
    -key ${client_ca_priv} -CA ${client_root_cert} -CAkey ${client_root_priv} -out ${client_ca_cert}
echo "Generate client"
openssl req \
    -config ${cfg} -x509 -section req_client -extensions v3_client \
    -key ${client_priv} -CA ${client_ca_cert} -CAkey ${client_ca_priv} -out ${client_cert}

cat ${client_cert} ${client_ca_cert} > ${client_chain}

# convert iso key to PEM
openssl asn1parse -genconf iso_pkey.asn1 -noout -out -| openssl pkey -inform der -out iso_priv.pem
