# Copyright Open Logistics Foundation
# 
# Licensed under the Open Logistics Foundation License 1.3.
# For details on the licensing terms, see the LICENSE file.
# SPDX-License-Identifier: OLFL-1.3
# 
# This Dockerfile encapsulates the whole build process of the libVDA5050++
# 

FROM ubuntu:jammy


# Install build dependencies
RUN apt update -y && apt install -y \
  build-essential\
  catch2 \
  clang-14 \
  cmake \
  cmake-curses-gui \
  doxygen \
  git \
  libfmt-dev \
  libpaho-mqtt-dev \
  libpaho-mqttpp-dev \
  libspdlog-dev \
  libssl-dev \
  ninja-build \
  nlohmann-json3-dev \
  openssl \
  sudo \
  && apt autoremove -y && apt clean

COPY default-build default-deb /usr/bin/
RUN ["chmod", "+x", "/usr/bin/default-build"]
