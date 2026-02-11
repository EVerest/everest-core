#!/bin/bash
set -e # exit on any error
dockerize -wait tcp://ocpp-db:3306 -timeout 60s

if [ ! -f ".buildsuccess" ]; then
  mvn clean package -Pdocker -Djdk.tls.client.protocols="TLSv1,TLSv1.1,TLSv1.2"
  touch .buildsuccess
fi

java -jar target/steve.jar