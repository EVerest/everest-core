# EVerest with docker

You can build docker images of everest-core or out-of-tree repositories using the provided utility script. 

## Build an EVerest docker image

With 

```bash
./build.sh [--conf <string>]
```

a docker image of everest-core will be created using the given EVerest configuration file.

Specify the following options to create your desired docker image of EVerest:
  
* repo: Git repository (e.g. https://github.com/EVerest/everest-core.git) - Optional, defaults to: https://github.com/EVerest/everest-core.git
* branch: Git branch or tag name (e.g main or 2024.6.0) - Optional, defaults to: main
* conf: Path to EVerest config file (e.g. /home/$(whoami)/checkout/everest-workspace/everest-core/config/ config-sil.yaml) - Required.
* ocpp-conf: Path to EVerest OCPP config file (e.g. /home/$(whoami)/checkout/everest-workspace/libocpp/aux/config-docker.json) - Optional, defaults to: ocpp-config.json
* name: Name of the docker image (e.g everest-core) - Optional, defaults to: everest-core
* build-date: Build date of the docker image, is reflected in its name and can have an effect on caching - Optional, defaults to the current datetime

```bash
./build.sh [--repo <GIT-REPOSITORY>] [--branch <BRANCH-NAME>] [--conf <EVEREST-CONFIG>] [--ocpp-conf <OCPP-CONFIG>] [--name <IMAGE-NAME>] [--build-date 2042]
```
Remember to provide an ocpp configuration file if you use an EVerest config that loads the OCPP module. Be aware that the provided OCPP config file will always be named `ocpp-config.json` inside the docker container. Consider this when configuring the OCPP module within the EVerest config and to set the ChargePointConfigPath accordingly.

Images will be created in a .tar.gz format in this folder.

## Run EVerest in docker

To run the image in a docker container, run the following commands

```bash
docker load < <YOUR_IMAGE-TIMESTAMP.tar.gz>
docker run --rm -it --network host <IMAGE_NAME>
```

## Run EVerest with OCPP 2.0.1 in docker
To run EVerest with OCPP 2.0.1 you can use the provided docker-compose.yaml with the example run-config-fallback.sh script
