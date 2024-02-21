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
  ca-certificates \
  catch2 \
  clang-14 \
  cmake \
  cmake-curses-gui \
  doxygen \
  git \
  gpg \
  libssl-dev \
  llvm \
  ninja-build \
  nlohmann-json3-dev \
  openssl \
  python3-pip \
  sudo \
  unzip \
  wget \
  && apt autoremove -y && apt clean

# Add kitware's repo key
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

# Add kitware repo for cmake >= 3.25
RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null

# Install cmake >= 3.25
RUN apt update -y && apt install -y cmake \
  && apt autoremove -y && apt clean

# Install mkdocs
RUN pip3 install mkdocs

# Setup SonarQube
WORKDIR /
RUN wget https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.6.2.2472-linux.zip \
    && unzip sonar-scanner-cli-4.6.2.2472-linux.zip \
    && rm sonar-scanner-cli-4.6.2.2472-linux.zip
ENV PATH="${PATH}:/sonar-scanner-4.6.2.2472-linux/bin"
