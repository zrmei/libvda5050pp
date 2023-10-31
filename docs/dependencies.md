# Introduction

All dependencies except [PahoMqttCpp](#pahomqttcpp) will be automatically downloaded via [CPM](https://github.com/cpm-cmake/CPM.cmake). If you have some of the dependencies
installed, you might want to check if the versions match, since the libVDA5050++ searches
for exact version matches. If you intend to use your system installation, please use the
`LIBVDA5050PP_<dep>_VERSION` CMake flags to overwrite the version to be searched (see [Install/Configuration Options](install.md#configuration-options)).

# PahoMqttCpp (manual)

Can be found in this [Repository](https://github.com/eclipse/paho.mqtt.cpp), published under the [Eclipse Public License 1.0](https://github.com/eclipse/paho.mqtt.cpp/blob/master/epl-v10).

| CPM Version | APT Version |
| ----------- | ----------- |
| -           | `1.2.0`     |

Install options:

- `apt install libpaho-mqtt-dev libpaho-mqttpp-dev openssl libssl-dev`
- CMake install from source

#### CMake install from source instructions

Prerequisites:

```sh
apt install libssl-dev
```

Build and install paho.mqtt.c:

```shell
git clone --depth 1 --branch v1.3.9 https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```

Optionally add the `-DCMAKE_INSTALL_PREFIX=$HOME/.local` flag to install
the library locally.


Build and install paho.mqtt.cpp:

```shell
git clone --depth 1 --branch v1.2.0 https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
  -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

If the `paho.mqtt.c` library was installed locally, add the `-DCMAKE_PREFIX_PATH=$HOME/.local` flag,
to locate it.

Optionally add the `-DCMAKE_INSTALL_PREFIX=$HOME/.local` flag to install
the (paho.mqtt.cpp) library locally.

# Eventpp (automatic)

Can be found in this [Repository](https://github.com/wqking/eventpp), published under the [Apache License V2.0](http://www.apache.org/licenses/LICENSE-2.0).

| CPM Version | APT Version |
| ----------- | ----------- |
| `0.1.2`     | -           |

Install options:

- Automatic CPM include
- CMake install from source

# FMT (automatic)

Can be found in this [Repository](https://github.com/fmtlib/fmt), published under a [Custom License](https://github.com/fmtlib/fmt/blob/master/LICENSE).

| CPM Version | APT Version |
| ----------- | ----------- |
| `8.1.1`     | `8.1.1`     |

Install options:

- Automatic CPM include
- CMake install from source
- `apt install libfmt-dev`

# spdlog (automatic)

Can be found in this [Repository](https://github.com/gabime/spdlog), published under the [MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE).

| CPM Version | APT Version |
| ----------- | ----------- |
| `1.10.0`    | `1.9.2`     |

Install options:

- Automatic CPM include
- CMake install from source
- `apt install libspdlog-dev`

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
| `3.11.2`    | `3.10.5`    |

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
