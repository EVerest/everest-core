FROM maven:3.6.1-jdk-11

ENV LANG=C.UTF-8 LC_ALL=C.UTF-8

WORKDIR /steve

ENV DOCKERIZE_VERSION v0.6.1
RUN wget --no-verbose https://github.com/jwilder/dockerize/releases/download/$DOCKERIZE_VERSION/dockerize-linux-amd64-$DOCKERIZE_VERSION.tar.gz \
    && tar -C /usr/local/bin -xzvf dockerize-linux-amd64-$DOCKERIZE_VERSION.tar.gz \
    && rm dockerize-linux-amd64-$DOCKERIZE_VERSION.tar.gz

RUN wget -qO- https://github.com/steve-community/steve/archive/steve-3.6.0.tar.gz | tar xz --strip-components=1
COPY main.properties src/main/resources/config/docker
COPY init.sh .
COPY keystore.jks .

ARG TARGETARCH
COPY entrypoint_wrapper.sh /entrypoint_wrapper.sh
RUN if [ ${TARGETARCH} != "amd64" ]; then \
        ln -s /usr/local/bin/mvn-entrypoint.sh /wrapped_entrypoint.sh; \
        cp /entrypoint_wrapper.sh /entrypoint.sh; \
    else \
        ln -s /usr/local/bin/mvn-entrypoint.sh /entrypoint.sh; \
    fi; \
    rm /entrypoint_wrapper.sh
ENTRYPOINT [ "/entrypoint.sh" ]
CMD /steve/init.sh



