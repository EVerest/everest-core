FROM debian:bullseye-slim

ARG BASE_IMAGE
ARG USERNAME=docker
ARG USER_UID=1000

# run update
RUN apt-get update 

# install necessary packages
RUN apt-get install --no-install-recommends -y \
    sudo \
    python3 \
    python3-pip \
    python-is-python3 \
    git \
    ssh \
    make

# create user and add to sudoers
RUN useradd -ms /bin/bash -u ${USER_UID} -U ${USERNAME} \
    && echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME}

# install sphinx related
RUN pip3 install \
    sphinx==5.3.0 \
    sphinx_rtd_theme==0.5.1 \
    doc8==1.0.0 \
    esbonio==0.14.3

# install latex for sphinx
RUN apt-get install  --no-install-recommends -y \
    latexmk \
    texlive-latex-recommended \
    texlive-latex-extra \
    tex-gyre

# clean up
RUN apt-get clean

# EVerest docu additions
RUN pip3 install \
    sphinxcontrib-contentui==0.2.5 \
    PyYAML==6.0

USER ${USERNAME}

ENV PATH="/home/${USERNAME}/.local/bin:${PATH}"

