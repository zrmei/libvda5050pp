# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 

# INFO: If you change this file, make sure to update ./version.yml

FROM ubuntu:jammy


# Install build dependencies
RUN apt update -y && apt install -y \
  build-essential\
  catch2 \
  clang-14 \
  cmake \
  doxygen \
  git \
  libpaho-mqtt-dev \
  libpaho-mqttpp-dev \
  libssl-dev \
  ninja-build \
  nlohmann-json3-dev \
  openssl \
  python3-pip \
  sudo \
  && apt autoremove -y && apt clean

# Install mkdocs
RUN pip3 install mkdocs