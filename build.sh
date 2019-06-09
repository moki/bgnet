#!/bin/bash

OPTIND=1

DIRS[0]="5-system_call-or-bust"
DIRS[1]="6-client-server-bg/server-tcp"
DIRS[2]="6-client-server-bg/client-tcp"

ROOT_DIR="$(pwd)"
OPT_CLEAN="clean"

build() {
	echo "building..."
	cd "$1"
	make
	cd "${ROOT_DIR}"
}

clean() {
	echo "cleaning..."
	cd "$1"
	make clean
	cd "${ROOT_DIR}"
}

for arg in "$@"; do
	if [[ $arg == "$OPT_CLEAN" ]]
	then
		for dir in "${DIRS[@]}"; do
			clean "${dir}"
		done
	fi

	for dir in "${DIRS[@]}"; do
		build "${dir}"
	done
done
