#!/bin/sh

base=.
cfg=./openssl-pki.conf
tpm=$1

if [ -z "$tpm" ]; then
    dir=pki
else
    dir=tpm_pki
fi

[ ! -f "$cfg" ] && echo "missing openssl-pki.conf" && exit 1

generate() {
    local base=$1
    local dir=$2
    mkdir -p ${base}/${dir}

    local root_priv=${base}/${dir}/root_priv.pem
    local ca_priv=${base}/${dir}/ca_priv.pem
    local server_priv=${base}/${dir}/server_priv.pem

    local root_cert=${base}/${dir}/root_cert.pem
    local ca_cert=${base}/${dir}/ca_cert.pem
    local server_cert=${base}/${dir}/server_cert.pem
    local cert_path=${base}/${dir}/cert_path.pem

    local tpmA tpmB
    local propA propB
    if [ -n "$3" ]; then
        tpmA="-provider"
        tpmB="tpm2"
        propA="-propquery"
        propB="?provider=tpm2"
    fi

    # generate keys
    for i in ${root_priv} ${ca_priv} ${server_priv}
    do
        openssl genpkey -config ${cfg} ${tpmA} ${tpmB} ${propA} ${propB} -algorithm RSA -pkeyopt bits:2048 -out $i
    done

    export OPENSSL_CONF=${cfg}
    # generate root cert
    echo "Generate root"
    openssl req ${tpmA} ${tpmB} -provider default ${propA} ${propB} \
        -config ${cfg} -x509 -section req_root -extensions v3_root \
        -key ${root_priv} -out ${root_cert}
    # generate ca cert
    echo "Generate ca"
    openssl req ${tpmA} ${tpmB} -provider default ${propA} ${propB} \
        -config ${cfg} -x509 -section req_ca -extensions v3_ca \
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

generate $base $dir $tpm
