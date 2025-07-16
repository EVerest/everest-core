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
