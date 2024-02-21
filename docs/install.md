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

### Prerequisites

- `c++17` compiler (like `clang>=11` or `g++>=9`)
- [CMake](https://cmake.org/) version `>=3.25` (Ubuntu 22.04 only provides `3.22`, see [official APT repository](https://apt.kitware.com/))
- OpenSSL `sudo apt install openssl libssl-dev`
- Optionally check [dependencies](dependencies.md) for manual installation
  of other packages

### Installation 

You can build and install the libVDA5050++ via CMake on your system.
A working `c++17` compiler like `clang` or `gcc` is required to build the library.
First you have to make sure [CMake](https://cmake.org/) is installed.
Most [dependencies](dependencies.md) will be downloaded via [CPM](https://github.com/cpm-cmake/CPM.cmake), if not already found on your system. Paho requires `OpenSSL`, it needs to be installed on your system.

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

### MSVC

The `libVDA5050++` is developed mainly for Linux based systems. However it's possible to build it on Windows with `MSVC`.
Even though this is not a clean approach it works and will be improved in the future. Most dependencies work just fine
with CPM and since Paho `v1.3.2` CPM support is better, but not yet optimal. If you intend to build the library on Windows,
you will need:

- MSVC for at least `c++17`
- [CMake](https://cmake.org/download/) for Windows
- [OpenSSL](https://github.com/openssl/openssl) _Note: There are many ways to install OpenSSL, some are described below_

OpenSSL via winget:
```powershell
# Install openssl via winget
winget search openssl # select a package
winget install <package>

cd libvda5050pp

cmake
  -Bbuild
  -G "Visual Studio 17 2022" # select your msvc generator
  -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE # Required since export keywords are not used on linux
  -A x64 # Set architecture
  -DLIBVDA5050PP_ENABLE_W_FLAGS=OFF # Optionally disable -W* flags, since the compilation will yield many infos ect.
  -DOPENSSL_ROOT_DIR='<your_openssl_root_dir>' # Set the root dir to your openssl installation (can be in the environment, too)
cmake --build build --target ALL_BUILD
```

OpenSSL via vcpkg:
```powershell
cd libvda5050pp

# Install openssl with vcpkg
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install openssl:x64-windows

cmake
  -Bbuild
  -G "Visual Studio 17 2022" # select your msvc generator
  -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE # Required since export keywords are not used on linux
  -A x64 # Set architecture
  -DLIBVDA5050PP_ENABLE_W_FLAGS=OFF # Optionally disable -W* flags, since the compilation will yield many infos ect.
  -DCMAKE_TOOLCHAIN_FILE="./vcpkg/scripts/buildsystems/vcpkg.cmake" # Use vcpkg toolchain file
cmake --build build --target ALL_BUILD
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
| `LIBVDA5050PP_ENABLE_W_FLAGS`                    | Enable all sorts of -W flags (default `ON`) otherwise use `-w`                                                |
| `LIBVDA5050PP_EVENTPP_VERSION`                   | Overwrite the Eventpp Version                                               |
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