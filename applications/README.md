# Applications related to EVerest

This directory will contain applications that are used during the EVerest build process,
deployed to the target hardware and useful scripts for development.

## containers/

containers/ contains multiple useful Dockerfile based containers that can be used for development.

* `mosquittto`      : An mqtt broker
* `mqtt-explorer`   : Tool to inspect messages on mqtt topics
* `nodered`         : NodeRED application to run i.e. sil flows
* `steve`           : OCPP backend

## dependency_manager

Contains the EDM (EVerest Dependency Manager) tool which is a cli to
manage an EVerest workspace and external dependencies during building.

## everest_dev_tool

A CLI designed to provide helpful commands/aliases for developing.

## utils/

utils/ contains ev-cli and everest-testing as well as additional code integrated from everest-utils.
This might get broken up further in the future.

## devrd

Script to manage devcontainer + containerized services in the development environment
