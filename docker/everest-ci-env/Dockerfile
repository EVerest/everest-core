ARG BASE_IMAGE=debian:11.0
FROM ${BASE_IMAGE}

RUN apt update && apt upgrade -y

RUN apt install -y \
    python3-pip \
    git \
    doxygen \
    wget \
    nodejs \
    latexmk \
    texlive-xetex \
    texlive-latex-recommended \
    texlive-latex-extra \
    texlive-fonts-recommended \
    fonts-freefont-otf \
    librsvg2-bin

RUN pip install -U sphinx
RUN pip install \
    sphinxcontrib-youtube \
    sphinxcontrib-contentui \
    sphinxcontrib-svg2pdfconverter \
    sphinx-notfound-page \
    setproctitle \
    jsonschema \
    commentjson \
    coloredlogs \
    pyyaml

ARG EV_DEV_TOOLS_VERSION=v0.1.0
RUN pip install "$(echo "ev-dev-tools @ git+https://github.com/EVerest/everest-utils@${EV_DEV_TOOLS_VERSION}#subdirectory=ev-dev-tools")"
ARG EDM_TOOL_VERSION=v0.5.3
RUN pip install "$(echo "edm-tool @ git+https://github.com/EVerest/everest-dev-environment@${EDM_TOOL_VERSION}#subdirectory=dependency_manager")"
