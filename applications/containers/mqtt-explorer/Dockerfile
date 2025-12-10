FROM smeagolworms4/mqtt-explorer:browser-1.0.3

COPY ./settings.json /mqtt-explorer/config/settings.json

ARG TARGETARCH
COPY entrypoint_wrapper.sh /entrypoint_wrapper.sh
RUN if [ ${TARGETARCH} != "amd64" ]; then \
        mv /entrypoint.sh /wrapped_entrypoint.sh; \
        cp /entrypoint_wrapper.sh /entrypoint.sh; \
    fi; \
    rm /entrypoint_wrapper.sh
ENTRYPOINT [ "/entrypoint.sh" ]
CMD node node-server/server/dist/node-server/server/src/index.js \
   --http-port=$HTTP_PORT \
   --config-path=$CONFIG_PATH \
   --http-user=$HTTP_USER \
   --http-password=$HTTP_PASSWORD\
   --ssl-key-path=$SSL_KEY_PATH\
   --ssl-cert-path=$SSL_CERT_PATH
