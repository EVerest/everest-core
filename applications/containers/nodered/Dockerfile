FROM nodered/node-red:2.2.3
RUN npm install node-red-dashboard@3.6.5
RUN npm install node-red-contrib-ui-actions@0.1.8
RUN npm install node-red-node-ui-table@0.4.3
RUN npm install node-red-contrib-ui-level@0.1.46

USER root
COPY entrypoint.sh /entrypoint.sh
ARG TARGETARCH
COPY entrypoint_wrapper.sh /entrypoint_wrapper.sh
RUN if [ ${TARGETARCH} != "amd64" ]; then \
        mv /entrypoint.sh /wrapped_entrypoint.sh; \
        cp /entrypoint_wrapper.sh /entrypoint.sh; \
    fi; \
    rm /entrypoint_wrapper.sh
USER node-red
ENTRYPOINT [ "/entrypoint.sh" ]
