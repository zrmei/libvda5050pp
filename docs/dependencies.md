# Introduction

All dependencies except OpenSSL will be automatically downloaded via [CPM](https://github.com/cpm-cmake/CPM.cmake). If you have some of the dependencies installed, you might want to check if the versions match, since the libVDA5050++ searches
for exact version matches. If you intend to use your system installation, please use the
`LIBVDA5050PP_<dep>_VERSION` CMake flags to overwrite the version to be searched (see [Install/Configuration Options](install.md#configuration-options)).

# PahoMqttCpp (automatic)

Can be found in this [Repository](https://github.com/eclipse/paho.mqtt.cpp), published under the [Eclipse Public License 1.0](https://github.com/eclipse/paho.mqtt.cpp/blob/master/epl-v10).

| CPM Version | APT Version |
| ----------- | ----------- |
| `1.3.2`     | `1.2.0`     |

Since version `1.3.2` we were able to install Paho via CPM, however this approach is not yet optimal (related [Issue](https://github.com/eclipse/paho.mqtt.cpp/issues/469))
and requires a little patch applied via CPM directly. This will probably be resolved in future Paho versions.

# Eventpp (automatic)

Can be found in this [Repository](https://github.com/wqking/eventpp), published under the [Apache License V2.0](http://www.apache.org/licenses/LICENSE-2.0).

| CPM Version | APT Version |
| ----------- | ----------- |
| `0.1.3`     | -           |

Install options:

- Automatic CPM include
- CMake install from source

# FMT (automatic)

Can be found in this [Repository](https://github.com/fmtlib/fmt), published under a [Custom License](https://github.com/fmtlib/fmt/blob/master/LICENSE).

Install options:

- Automatic include as a dependency of spdlog

# spdlog (automatic)

Can be found in this [Repository](https://github.com/gabime/spdlog), published under the [MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE).

| CPM Version | APT Version |
| ----------- | ----------- |
| `1.10.0`    | -           |

Install options:

- Automatic CPM include
- CMake install from source

# Tomlplusplus (automatic)

Can be found in this [Repository](https://github.com/marzer/tomlplusplus), published under the [MIT License](https://github.com/marzer/tomlplusplus/blob/master/LICENSE).

| CPM Version | APT Version |
| ----------- | ----------- |
| `3.3.0`     | -           |

Install options:

- Automatic CPM include
- CMake install from source

# NLohmann JSON (automatic)
Can be found in this [Repository](https://github.com/nlohmann/json), published under the [MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT).

| CPM Version | APT Version |
| ----------- | ----------- |
| `3.11.3`    | `3.10.5`    |

Install options:

- Automatic CPM include
- CMake install from source
- `apt install nlohmann-json3-dev`

# Catch2 (automatic)
Can be found in this [Repository](https://github.com/catchorg/Catch2), published under the [Boost Software License 1.0](https://github.com/catchorg/Catch2/blob/devel/LICENSE.txt).

| CPM Version | APT Version |
| ----------- | ----------- |
| `2.13.10`   | `2.13.8`    |

Install options:

- Automatic CPM include
- CMake install from source
- `apt install catch2`
