# syntax=docker/dockerfile:1
ARG BASE_IMAGE_TAG=latest
FROM ghcr.io/everest/everest-ci/build-kit-base:${BASE_IMAGE_TAG}

RUN apt update \
    && apt install --no-install-recommends -y \
    python3-build=0.9.0-1

RUN apt clean \
    && rm -rf /var/lib/apt/lists/*
