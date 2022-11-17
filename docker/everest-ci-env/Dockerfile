ARG BASE_IMAGE=debian:11.0
FROM ${BASE_IMAGE}

RUN apt update && apt upgrade -y

RUN apt install -y \
    python3-pip \
    git \
    doxygen \
    wget \
    nodejs

RUN pip install -U sphinx
RUN pip install \
    sphinxcontrib-contentui \
    setproctitle \
    jsonschema \
    commentjson \
    coloredlogs \
    pyyaml
