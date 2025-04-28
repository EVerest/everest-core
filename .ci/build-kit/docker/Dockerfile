# syntax=docker/dockerfile:1
ARG BASE_IMAGE_TAG=latest
FROM ghcr.io/everest/everest-ci/build-kit-base:${BASE_IMAGE_TAG}

ARG GO_VERSION=1.24.2
ADD https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz /tmp/go.tar.gz
RUN if [ -d /usr/local/go ]; then \
        echo "Go is already installed"; \
    else \
        tar -C /usr/local -xzf /tmp/go.tar.gz; \
    fi
ENV PATH="/usr/local/go/bin:${PATH}"
RUN go version

ENV GRPC_VERSION=v1.71.0
ENV GRPC_EXTENDED_CPP_PLUGIN_VERSION=main
RUN tmpdir=$(mktemp -d) \
    && cd $tmpdir \
    && git clone --depth 1 --recurse-submodules --shallow-submodules --branch ${GRPC_VERSION} https://github.com/grpc/grpc.git \
    && git clone --branch ${GRPC_EXTENDED_CPP_PLUGIN_VERSION} https://github.com/EVerest/grpc-extended-cpp-plugin.git \
    && cmake -B build -S grpc-extended-cpp-plugin -G Ninja -D gRPC_INSTALL=ON \
    && cmake --build build \
    && cmake --install build \
    && rm -rf $tmpdir

RUN apt update \
    && apt install -y \
        build-essential \
        zlib1g-dev \
        libncurses5-dev \
        libgdbm-dev \
        libnss3-dev \
        libssl-dev \
        libreadline-dev \
        libffi-dev \
        libsqlite3-dev \
        wget
ENV PYTHON_VERSION=3.13.3
ENV PYTHON_SOURCE_URL=https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz
RUN tmpdir=$(mktemp -d) \
    && cd $tmpdir \
    && python3 -m pip freeze > ${tmpdir}/requirements.txt \
    && wget ${PYTHON_SOURCE_URL} \
    && tar -xzvf Python-${PYTHON_VERSION}.tgz \
    && cd Python-${PYTHON_VERSION} \
    && ./configure --enable-optimizations \
    && make -j $(nproc) \
    && make install \
    && short_version=$(echo ${PYTHON_VERSION} | cut -d. -f1-2) \
    && rm /usr/local/bin/python3 \
    && ln -s /usr/local/bin/python${short_version} /usr/local/bin/python3 \
    && python3 -m pip install --break-system-packages -r ${tmpdir}/requirements.txt

RUN python3 -m pip install --break-system-packages \
    grpcio \
    grpcio-tools

# Can be used to use an other version of everest-cmake
ADD "https://www.random.org/cgi-bin/randbyte?nbytes=10&format=h" skipcache
ENV EVEREST_CMAKE_PATH=/usr/lib/cmake/everest-cmake
ENV EVEREST_CMAKE_VERSION=tmp/eebus
RUN rm -rf ${EVEREST_CMAKE_PATH} \
    && git clone https://github.com/EVerest/everest-cmake.git ${EVEREST_CMAKE_PATH} \
    && cd ${EVEREST_CMAKE_PATH} \
    && git checkout ${EVEREST_CMAKE_VERSION} \
    && rm -r .git
