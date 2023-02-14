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
    setproctitle \
    jsonschema \
    commentjson \
    coloredlogs \
    pyyaml

RUN pip install 'ev-dev-tools @ git+https://github.com/EVerest/everest-utils@main#subdirectory=ev-dev-tools'
RUN pip install 'edm-tool @ git+https://github.com/EVerest/everest-dev-environment@main#subdirectory=dependency_manager'
