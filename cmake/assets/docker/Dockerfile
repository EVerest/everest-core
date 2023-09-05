FROM nodered/node-red:2.2.3

# run as root because below commands (sometimes?) return:
# 
# npm ERR! Your cache folder contains root-owned files, due to a bug in 
# npm ERR! previous versions of npm which has since been addressed.
#
# See https://github.com/node-red/node-red-docker/tree/master#user-permission-errors
USER root

RUN npm install node-red-dashboard
RUN npm install node-red-contrib-ui-actions
RUN npm install node-red-node-ui-table
RUN npm install node-red-contrib-ui-level
