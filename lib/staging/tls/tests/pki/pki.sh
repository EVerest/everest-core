#!/bin/sh

generate() {
    local base=$1
    # generate keys
    for i in    ${base}${server_root_priv} ${base}${server_ca_priv} ${base}${server_priv} \
                ${base}${client_root_priv} ${base}${client_ca_priv} ${base}${client_priv}
    do
        openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out "$i"
        chmod 644 "$i"
    done

    export OPENSSL_CONF=${base}${cfg}

    echo "Generate ${base}server_root"
    openssl req \
        -config ${base}${cfg} -x509 -section req_server_root -extensions v3_server_root \
        -key ${base}${server_root_priv} -out ${base}${server_root_cert}
    echo "Generate ${base}server_ca"
    openssl req \
        -config ${base}${cfg} -x509 -section req_server_ca -extensions v3_server_ca \
        -key ${base}${server_ca_priv} -CA ${base}${server_root_cert} -CAkey ${base}${server_root_priv} -out ${base}${server_ca_cert}
    echo "Generate ${base}server"
    openssl req \
        -config ${base}${cfg} -x509 -section req_server -extensions v3_server \
        -key ${base}${server_priv} -CA ${base}${server_ca_cert} -CAkey ${base}${server_ca_priv} -out ${base}${server_cert}
    cat ${base}${server_cert} ${base}${server_ca_cert} > ${base}${server_chain}

    echo "Generate ${base}client_root"
    openssl req \
        -config ${base}${cfg} -x509 -section req_client_root -extensions v3_client_root \
        -key ${base}${client_root_priv} -out ${base}${client_root_cert}
    echo "Generate ${base}client_ca"
    openssl req \
        -config ${base}${cfg} -x509 -section req_client_ca -extensions v3_client_ca \
        -key ${base}${client_ca_priv} -CA ${base}${client_root_cert} -CAkey ${base}${client_root_priv} -out ${base}${client_ca_cert}
    echo "Generate ${base}client"
    openssl req \
        -config ${base}${cfg} -x509 -section req_client -extensions v3_client \
        -key ${base}${client_priv} -CA ${base}${client_ca_cert} -CAkey ${base}${client_ca_priv} -out ${base}${client_cert}

    cat ${base}${client_cert} ${base}${client_ca_cert} > ${base}${client_chain}
}

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

generate
generate alt_

# cross signed intermediate certificate
echo "Generate cross_ca"
openssl req \
    -config ${cfg} -x509 -section req_server_ca -extensions v3_server_ca \
    -key ${base}${server_ca_priv} -CA ${base}${client_root_cert} -CAkey ${base}${client_root_priv} -out cross_ca_cert.pem

# convert iso key to PEM
openssl asn1parse -genconf iso_pkey.asn1 -noout -out -| openssl pkey -inform der -out iso_priv.pem
