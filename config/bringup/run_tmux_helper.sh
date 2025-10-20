#!/bin/bash

PREFIX=$2
EVEREST_CONFIG_FILE=$1

function parse_yaml {
   local prefix=$2
   local s='[[:space:]]*' w='[a-zA-Z0-9_]*' fs=$(echo @|tr @ '\034')
   sed -ne "s|^\($s\):|\1|" \
        -e "s|^\($s\)\($w\)$s:$s[\"']\(.*\)[\"']$s\$|\1$fs\2$fs\3|p" \
        -e "s|^\($s\)\($w\)$s:$s\(.*\)$s\$|\1$fs\2$fs\3|p"  $1 |
   awk -F$fs '{
      indent = length($1)/2;
      vname[indent] = $2;
      for (i in vname) {if (i > indent) {delete vname[i]}}
      if (length($3) > 0) {
         vn=""; for (i=0; i<indent; i++) {vn=(vn)(vname[i])("_")}
         printf("%s%s%s=\"%s\"\n", "'$prefix'",vn, $2, $3);
      }
   }'
}

if [ $# -lt 2 ]; then
    echo "Usage: $0 config_file prefix"
    echo "    config: config to be used to run EVerest"
    echo "    prefix: install prefix for this EVerest instance"
    exit
fi

LINES=$(parse_yaml $EVEREST_CONFIG_FILE | grep "standalone=\"true\"")

PAT="_standalone=\"true\""
PAT2="active_modules_"

for LINE in $LINES;  do
  MODULE_ID=$(echo $LINE | sed -n "/$PAT/s/$PAT//p" - | sed -n "/$PAT2/s/$PAT2//p" -)
  IDS+=( $MODULE_ID )
  PAT3=$PAT2$MODULE_ID"_module="
  MODULE_NAME=$(parse_yaml $EVEREST_CONFIG_FILE | grep $PAT3 | sed -n "/$PAT3/s/$PAT3//p" - | sed -r "s/[\"]+//g" -)
  NAMES+=( $MODULE_NAME )
done

tmux new-session -d -s EVerest
tmux set -g mouse on

tmux send -t EVerest:0.0 "$PREFIX/bin/manager --prefix $PREFIX" SPACE "--conf " $EVEREST_CONFIG_FILE ENTER
LEN=${#NAMES[@]}

for (( j=0; j<$LEN; j++ ));
do
  if [ $j -gt 0 ]
  then
    tmux split-window -t EVerest:0.$j -v
  else
    tmux split-window -t EVerest:0.0 -h
  fi
  tmux send -t EVerest:0.$((j+1)) "sleep 1 &&" SPACE "$PREFIX/libexec/everest/modules/" ${NAMES[$j]} "/" ${NAMES[$j]} SPACE \
    "--module" SPACE ${IDS[$j]} ENTER
done
tmux a
tmux select-layout even-vertical
tmux kill-session -t EVerest
