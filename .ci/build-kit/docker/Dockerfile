# syntax=docker/dockerfile:1
ARG BASE_IMAGE_TAG=latest
FROM ghcr.io/everest/everest-ci/build-kit-base:${BASE_IMAGE_TAG}

# Can be used to use an other version of everest-cmake
ADD "https://www.random.org/cgi-bin/randbyte?nbytes=10&format=h" skipcache
ENV EVEREST_CMAKE_PATH=/usr/lib/cmake/everest-cmake
ENV EVEREST_CMAKE_VERSION=tmp/eebus
RUN rm -rf ${EVEREST_CMAKE_PATH} \
    && git clone https://github.com/EVerest/everest-cmake.git ${EVEREST_CMAKE_PATH} \
    && cd ${EVEREST_CMAKE_PATH} \
    && git checkout ${EVEREST_CMAKE_VERSION} \
    && rm -r .git

ARG GO_VERSION=1.24.2
ADD https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz /tmp/go.tar.gz
RUN if [ -d /usr/local/go ]; then \
        echo "Go is already installed"; \
    else \
        tar -C /usr/local -xzf /tmp/go.tar.gz; \
    fi
ENV PATH="/usr/local/go/bin:${PATH}"
RUN go version

RUN pip install --break-system-packages \
    grpcio \
    grpcio-tools
