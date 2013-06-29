YAJL build system is basaed on [CMake](http://www.cmake.org).

1. Chose and create a nice directory to store the intermediate build files.
2. Execute `cmake -G"Unix Makefiles" -D CMAKE_INSTALL_PREFIX=./install -D GENERATE_DOCUMENTATION=ON path/to/yajl/root/dir`;
    - `-G"Unix Makefiles"` tells cmake to generate makefiles, other generators are available depending on your platform,
    - `-D CMAKE_INSTALL_PREFIX=./install` specify the installation directory to `./install`,
    - `-D GENERATE_DOCUMENTATION=ON` specify to not generate the documentation if [doxygen](http://www.stack.nl/~dimitri/doxygen/) is found,
    - `path/to/yajl/root/dir` tells cmake where it will find the root `CMakeLists.txt` it will build.
3. Build YAJL library and tests using your chosen build system, e.g. `make all`.
4. Execute `ctest -V` to launch the tests.
5. Execute `make install` to generate the package to the specified install directory.
6. Execute `cpack` to generate the `.tgz` package.

PS. obviously you can also use CMake via its gui.