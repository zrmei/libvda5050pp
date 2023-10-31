#!/bin/sh
# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 
# 

if [ ! -x /usr/bin/docker ]; then
  echo "Please install docker to use this script: https://docs.docker.com/get-docker/"
  exit 1
fi

SRC_DIR="$(readlink -e $(dirname $0)/..)"
BUILD_DIR="$SRC_DIR/docker/build"
OUTPUT_DIR="$1"
BUILD_SCRIPT="$2"
shift 2
FWD_ARGS="$@"

mkdir -vp "$OUTPUT_DIR"
mkdir -vp "$BUILD_DIR"

OUTPUT_DIR="$(readlink -e $OUTPUT_DIR)"

/usr/bin/docker run --rm \
  --mount type=bind,source=/etc/shadow,target=/etc/shadow,ro=true \
  --mount type=bind,source=/etc/passwd,target=/etc/passwd,ro=true \
  --mount type=bind,source="$SRC_DIR",target=/src \
  --mount type=bind,source="$OUTPUT_DIR",target=/install \
  --mount type=bind,source="$BUILD_DIR",target=/build \
  --env USER=$USER \
  libvda5050pp-build $BUILD_SCRIPT $FWD_ARGS