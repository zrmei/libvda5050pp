# Get the source code

The sources are managed via [git](https://git-scm.com/). Once you have installed git,
you can clone the repository with:
```shell
git clone https://git.openlogisticsfoundation.org/silicon-economy/libraries/vda5050/libvda5050pp.git
```

# Install the libVDA5050++ on Ubuntu 22.04

The only prerequisite you need is [Docker](https://www.docker.com/get-started/). Once
installed, you can start building the `.deb` Package.
The following command will setup a docker image called `libvda5050pp-build`:
```shell
docker/build-container.sh
```
You can use the image to  build the `.deb` package and copy it to the `.` directory with:
```shell
docker/run-container.sh . default-deb
```

# Build on your System

#### Prerequisites

- `c++17` compiler (like `clang>=11` or `g++>=9`)
- [CMake](https://cmake.org/)
- PahoMqttCpp
  - `sudo apt install libpaho-mqtt-dev libpaho-mqttpp-dev openssl libssl-dev`
  - or [build it from source](dependencies.md#pahomqttcpp)
- Optionally check [dependencies](dependencies.md) for manual installation
  of other packages

#### Installation 

You can build and install the libVDA5050++ via CMake on your system.
A working `c++17` compiler like `clang` or `gcc` is required to build the library.
First you have to make sure [CMake](https://cmake.org/) is installed.
Most [dependencies](dependencies.md) will be downloaded via [CPM](https://github.com/cpm-cmake/CPM.cmake), if not already found on your system. Since the
integration of [PahoMqttCpp](https://github.com/eclipse/paho.mqtt.cpp) is very clumsy with CPM, you need to install it by yourself. On Ubuntu you can install 
the packages `libpaho-mqtt-dev libpaho-mqttpp-dev openssl libssl-dev`,
on other systems you can [build it from source](dependencies.md#pahomqttcpp).

First configure the build to install the libVDA5050++ to a CMake findable `<install_prefix>` (for example `/usr/local` or `~/.local`):
```
cmake -Bbuild -DLIBVDA5050PP_INSTALL=ON -DCMAKE_INSTALL_PREFIX=<install_prefix>
```
Then install:
```shell
cmake --build build --target install
```

Note, that all [dependencies](dependencies.md), which are not already installed will automatically be
installed into the prefix by CMake.  You can enable a _clean_ installation with `-DLIBVDA5050PP_CLEAN_INSTALL=ON` to
remove the unused ones (which are already included in the `libvda5050++.so`), if you do this please check the `cmake/PostInstall.cmake` script, since
it can interfere with your existing installations.

Normal install scheme:

```
<install_prefix>
├── include
│   ├── eventpp
│   ├── nlohmann
│   ├── toml++
│   ├── vda5050
│   └── vda5050++
├── lib
│   ├── cmake
│   │   ├── eventpp
│   │   ├── libvda5050++
│   │   ├── tomlplusplus
│   │   └── vda5050_message_structs
│   ├── libvda5050_message_structs.a
│   ├── libvda5050++.so.0 -> libvda5050++.so.1.0.0
│   └── libvda5050++.so.1.0.0
└── share
    ├── cmake
    │   └── nlohmann_json
    ├── eventpp
    ├── pkgconfig
    └── tomlplusplus
```

Clean install scheme:
```
<install_prefix>
├── include
│   ├── vda5050
│   └── vda5050++
└── lib
    ├── cmake
    │   ├── libvda5050++
    │   └── vda5050_message_structs
    ├── libvda5050++.so.0 -> libvda5050++.so.1.0.0
    ├── libvda5050++.so.1.0.0
    └── x86_64-linux-gnu
        └── libvda5050_message_structs.a
```

### Configuration Options:

| Variable                                         | Description                                                                 |
| ------------------------------------------------ | --------------------------------------------------------------------------- |
| `LIBVDA5050PP_BUILD_DEB`                         | Enable `.deb` target                                                        |
| `LIBVDA5050PP_BUILD_DOCS`                        | Enable `mkdocs` target                                                      |
| `LIBVDA5050PP_BUILD_STATIC`                      | Build a static library instead of a dynamic one                             |
| `LIBVDA5050PP_CLEAN_INSTALL`                     | Enable _clean_ installation                                                 |
| `LIBVDA5050PP_INSTALL`                           | Generate install targets                                                    |
| `LIBVDA5050PP_USE_GLIBCXX_DEBUG`                 | Compile with **public** `-D_GLIBCXX_DEBUG` flag                             |
| `LIBVDA5050PP_CATCH2_VERSION`                    | Overwrite the Catch2 Version                                                |
| `LIBVDA5050PP_EVENTPP_VERSION`                   | Overwrite the Eventpp Version                                               |
| `LIBVDA5050PP_FMT_VERSION`                       | Overwrite the FMT Version                                                   |
| `LIBVDA5050PP_NLOHMANN_JSON_VERSION`             | Overwrite the nlohmann_json Version                                         |
| `LIBVDA5050PP_SPDLOG_VERSION`                    | Overwrite the spdlog Version                                                |
| `LIBVDA5050PP_TOMLPLUSPLUS_VERSION`              | Overwrite the TOMLPLUSPLUS Version                                          |
| `LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_VERSION`   | Overwrite the vda5050_message_structs Version                               |
| `LIBVDA5050PP_VDA5050_MESSAGE_STRUCTS_DEP_LOCAL` | Expect the vda5050_message_structs repo to be next to the libVDA5050++ repo |


# Build as a submodule

Another option is to build the libVDA5050++ as a submodule of your Adapter.
The prerequisites are the same, as in [Build on your System](#build-on-your-system).

1. Clone the libVDA5050++ repository in a folder of your Adapter like `third_party/libvda5050++`
2. Include the subdirectory with CMake:
```cmake
add_subdirectory(third_party/libvda5050++)
```


# Linking the libVDA5050++

With CMake, you can add the following lines in your `CMakeLists.txt`:
```cmake
find_package(libvda5050++ REQUIRED)
target_link_libraries(my_app PUBLIC libvda5050++::vda5050++)
```