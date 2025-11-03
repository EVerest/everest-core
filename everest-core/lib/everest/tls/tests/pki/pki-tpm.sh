#!/bin/sh

base=.
cfg=./openssl-pki.conf
dir=tpm_pki

[ ! -f "$cfg" ] && echo "missing openssl-pki.conf" && exit 1

generate() {
    local base=$1
    local dir=$2
    mkdir -p ${base}/${dir}

    local root_priv=${base}/${dir}/server_root_priv.pem
    local ca_priv=${base}/${dir}/server_ca_priv.pem
    local server_priv=${base}/${dir}/server_priv.pem

    local root_cert=${base}/${dir}/server_root_cert.pem
    local ca_cert=${base}/${dir}/server_ca_cert.pem
    local server_cert=${base}/${dir}/server_cert.pem
    local cert_path=${base}/${dir}/server_chain.pem

    local tpmA="-provider"
    local tpmB="tpm2"
    local propA="-propquery"
    local propB="?provider=tpm2"

    # generate keys
    for i in ${root_priv} ${ca_priv} ${server_priv}
    do
        openssl genpkey -config ${cfg} ${tpmA} ${tpmB} ${propA} ${propB} -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out $i
    done

    export OPENSSL_CONF=${cfg}
    # generate root cert
    echo "Generate root"
    openssl req ${tpmA} ${tpmB} -provider default ${propA} ${propB} \
        -config ${cfg} -x509 -section req_server_root -extensions v3_server_root \
        -key ${root_priv} -out ${root_cert}
    # generate ca cert
    echo "Generate ca"
    openssl req ${tpmA} ${tpmB} -provider default ${propA} ${propB} \
        -config ${cfg} -x509 -section req_server_ca -extensions v3_server_ca \
        -key ${ca_priv} -CA ${root_cert} \
        -CAkey ${root_priv} -out ${ca_cert}
    # generate server cert
    echo "Generate server"
    openssl req ${tpmA} ${tpmB} -provider default ${propA} ${propB} \
        -config ${cfg} -x509 -section req_server -extensions v3_server \
        -key ${server_priv} -CA ${ca_cert} \
        -CAkey ${ca_priv} -out ${server_cert}

    # create bundle
    cat ${server_cert} ${ca_cert} > ${cert_path}
}

generate $base $dir
