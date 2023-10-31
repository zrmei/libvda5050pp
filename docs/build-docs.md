# Build the Full Documentation

You need to install [Doxygen](https://www.doxygen.nl/) and [MkDocs](https://www.mkdocs.org/) to build this documentation.

### Install Mkdocs

To install `Mkdocs` on your system, execute the following command:

```sh
pip3 install mkdocs
```

If you don't have `python3` and `pip3` installed on your system you can also install mkdocs via `apt`. We encountered the problem that sometimes code blocks in the documentation are displayed as a single line with the `apt` version of `mkdocs`. Hence we recommend to install `mkdocs` with `pip3`.

```sh
apt install mkdocs
```

### Install Doxygen

To install `Doxygen` on your system, execute the following command:

```sh
apt install doxygen
```

### Build and view the Documentation

```shell
cmake -Bbuild -DLIBVDA5050PP_BUILD_DOCS=ON
cmake --build build --target mkdocs
cd build
mkdocs serve
```