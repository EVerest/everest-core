#!/bin/sh
#
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
#
# time windowed collection is supported as follows:
# - systemd journal supports "since" and "until" to specify the time window
# - OCPP logs are rotated daily and a log file is included when its modification
#   time falls between "since" and "until"
# - Completed session logs are included when their modification time falls 
#   between "since" and "until" (i.e. the end of session time is used)
# - In-progress session logs are included when their modification time falls 
#   between "since" and now i.e. where "until" is specified then in-progress
#   sessions are unlikely to be included 

error() {
    echo "$script error: $1" 1>&2
    exit 1
}

usage() {
    echo "Usage: $script <journal|ocpp|session>"
    echo "               [--dir /full/path/to/log/directory]"
    echo "               [--since \"yyyy-mm-dd hh:mm:ss\"]"
    echo "               [--until \"yyyy-mm-dd hh:mm:ss\"]"
    exit 2
}

section_start() {
    printf "### START $1\n"
}

section_end() {
    printf "\n### END $1\n"
}

# remove T from date/time strings
# 2023-12-18T16:06:03.435Z -> 2023-12-18 16:06:03.435Z
reformat_date() {
    pre=$(echo $1 | cut -c1-10)
    post=$(echo $1 | cut -c12-19)
    if [ -n "$pre" ] && [ -n "$post" ]; then
        echo "$pre $post"
    fi
}

# obtain the modification time of a file as a timestamp
modify_ts() {
    if [ -f "$1" ]; then
        stat -c %Y "$1"
    else
        echo 0
    fi
}

# convert the supplied date/time to a timestamp
date_ts() {
    if [ -n "$1" ]; then
        date -u +%s -d "$1"
    fi
}

# output file if modified between optional dates
# $1 file name
# $2 optional not before date
# $3 optional not after date
# $4 line to output before file
output_file() {
    if [ -f "$1" ]; then
        local modify_time=$(modify_ts "$1")
        local not_before=$(date_ts "$2")
        local not_after=$(date_ts "$3")
        local output=1
        [ -n "$not_before" ] && [ $modify_time -lt $not_before ] && output=0
        [ -n "$not_after" ] && [ $modify_time -gt $not_after ] && output=0
        if [ $output -ne 0 ]; then
            [ -n "$4" ] && echo "$4"
            sed -e '/^$/d' "$1"
        fi
    fi
}

# output systemd journal
# $1 optional not before date
# $2 optional not after date
#
# journalctl doesn't like empty arguments
do_journal() {
    section_start JOURNAL
    if [ -n "$1" ]; then
        if [ -n "$2" ]; then
            journalctl --output=json --no-pager "--since=$1" "--until=$2"
        else
            journalctl --output=json --no-pager "--since=$1"
        fi
    else
        journalctl --output=json --no-pager
    fi
    local res=$?
    section_end JOURNAL
    return $res
}

# output Everest OCPP logs
# $1 Everest OCPP log directory
# $2 optional not before date
# $3 optional not after date
do_ocpp() {
    [ -z "$1" ] && error "Missing OCPP log directory"
    local res=0
    section_start OCPP
    if [ -d "$1" ]; then
        find "$1" -name \*.log\* | sort -r | while read ocpp
        do
            output_file "$ocpp" "$2" "$3"
        done
        res=$?
    fi
    section_end OCPP
    return $res
}

# output Everest charging session logs
# $1 Everest session log directory
# $2 optional not before date
# $3 optional not after date
do_session() {
    [ -z "$1" ] && error "Missing session log directory"
    local name
    local res=0
    section_start SESSION
    if [ -d "$1" ]; then
        find "$1" -type d | sort -n | while read session
        do
            name=$(basename "${session}")
            if [ -f "${session}/eventlog.csv" ]; then
                output_file "${session}/eventlog.csv" "$2" "$3" "--- ${name}"
                res=$?
            fi
            if [ -f "${session}/incomplete-eventlog.csv" ]; then
                output_file "${session}/incomplete-eventlog.csv" "$2" "$3" "--- ${name}"
                res=$?
            fi
        done
    fi
    section_end SESSION
    return $res
}

script=$(basename $0)
[ -z "$1" ] && error "Missing sub-command"
cmd=$(basename "${1}")
shift

TEMP=$(getopt -o "" --long 'dir:,since:,until:' -n "$script" -- "$@")
if [ $? -ne 0 ]; then
    error "getopt parsing"
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

dir=
since=
until=

while true
do
    case "$1" in
        "--dir"|"--since"|"--until") eval "${1#--}=\"$2\""; shift 2; continue;;
        "--") shift; break;;
    esac
done

since=$(reformat_date "$since")
until=$(reformat_date "$until")

case $cmd in
    journal)    do_journal "$since" "$until";;
    ocpp)       do_ocpp "$dir" "$since" "$until";;
    session)    do_session "$dir" "$since" "$until";;
    *) usage;;
esac
