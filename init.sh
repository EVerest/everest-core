#!/bin/bash
# initialise a branch from external repos

add() {
    git -C ${subdir} subtree add --squash -P $1 git@github.com:EVerest/$1.git $2
}

update() {
    git -C ${subdir} subtree pull -m "update to release" --squash -P $1 git@github.com:EVerest/$1.git $2
}

subdir=.

repos="""
everest-cmake
everest-core
everest-framework
everest-sqlite
everest-utils
libcbv2g
libevse-security
libfsm
libiso15118
liblog
libocpp
libslac
libtimer
"""

declare -A initial
initial[everest-cmake]=a80ff2d606cf78efb8203fdd618bcbefeda87910
initial[everest-core]=c13c397e976fc0b2dce3c46b9c09d6a5139f5eca
initial[everest-framework]=0ad54a05d08153eee4b9593135872a7e630845f7
initial[everest-sqlite]=103ed84f81488ea8fca4c601312634e370e0d191
initial[everest-utils]=13bc452156fa1e722843519394ae3f6ad436a8ae
initial[libcbv2g]=ccf438dbac7cb7390434a90422bd4d9c0d0061fe
initial[libevse-security]=6c29e3b0b57e3275daa6584a77f28690e37ec0ee
initial[libfsm]=95e57dde5ba3423867063b0e20ee03c6414b13e2
initial[libiso15118]=f403aa812db047a37e575129040e0b6293f8442b
initial[liblog]=97a221e48328cdfd210172694501cfa63f976e48
initial[libocpp]=e8f4d23a262f4ba50a29b67c951cd8557ca9caac
initial[libslac]=785fb6153b1c9300ef36c2dedd40b4972748a44c
initial[libtimer]=c0a0af59d31592b81e5f537b733392f2c2a22bea

declare -A release
release[everest-cmake]=v0.5.4
release[everest-core]=2025.9.0
release[everest-framework]=v0.23.1
release[everest-sqlite]=v0.1.3
release[everest-utils]=v0.6.2
release[libcbv2g]=v0.3.1
release[libevse-security]=v0.9.9
release[libfsm]=v0.2.0
release[libiso15118]=v0.8.0
release[liblog]=v0.3.0
release[libocpp]=v0.30.2
release[libslac]=v0.3.0
release[libtimer]=v0.1.2

for i in ${repos}
do
    echo "${initial[$i]}->${release[$i]}"
    add $i ${initial[$i]}
    update $i ${release[$i]}
done
