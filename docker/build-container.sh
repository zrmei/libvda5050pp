#!/bin/sh

# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 
# 

if [ -x /usr/bin/docker ]; then
  /usr/bin/docker image build "$(dirname $0)" --tag libvda5050pp-build
else
  echo "Please install docker to use this script: https://docs.docker.com/get-docker/"
  exit 1
fi
