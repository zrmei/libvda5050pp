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
  ca-certificates \
  catch2 \
  clang-14 \
  doxygen \
  git \
  gpg \
  libssl-dev \
  ninja-build \
  nlohmann-json3-dev \
  openssl \
  sudo \
  wget \
  && apt autoremove -y && apt clean

# Add kitware's repo key
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

# Add kitware repo for cmake >= 3.25
RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null

# Install cmake >= 3.25
RUN apt update -y && apt install -y cmake \
  && apt autoremove -y && apt clean

COPY default-build default-deb /usr/bin/
RUN ["chmod", "+x", "/usr/bin/default-build"]
