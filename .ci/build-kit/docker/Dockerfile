# syntax=docker/dockerfile:1
ARG BASE_IMAGE_TAG=latest
FROM ghcr.io/everest/everest-ci/build-kit-base:${BASE_IMAGE_TAG}

# Can be used to use an other version of everest-cmake
# ENV EVEREST_CMAKE_PATH=/usr/lib/cmake/everest-cmake
# ENV EVEREST_CMAKE_VERSION=<everest-cmake-verion>
# RUN rm -rf ${EVEREST_CMAKE_PATH} \
#     && git clone https://github.com/EVerest/everest-cmake.git ${EVEREST_CMAKE_PATH} \
#     && cd ${EVEREST_CMAKE_PATH} \
#     && git checkout ${EVEREST_CMAKE_VERSION} \
#     && rm -r .git

# FIXME(kai): Install libsystemd-dev as a dependency of sdbus-c++
RUN apt-get update && apt-get -y install libsystemd-dev

# FIXME(andistorm): Install rust tools
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/${HOME}/.cargo/bin:${PATH}"
ENV PATH="/root/.cargo/bin:${PATH}"

RUN echo "Path: ${PATH}"
RUN which cargo || echo "Cargo not found in PATH"
