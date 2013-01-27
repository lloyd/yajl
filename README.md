=====================
YAJL 2
=====================

This is a fork of YAJL 2 (https://github.com/lloyd/yajl). It maily aims at
better platform building support.

Building
=====================

The following are cmake building options:

 * **DISABLE_DOCS**: Disable building docs
 * **DISABLE_TESTS**: Disable building tests and examples
 * **DISABLE_TOOLS**: Disable building tools
 * **DISABLE_STATIC**: Disable building static
 * **DISABLE_SHARED**: Disable building shared
 * **DISABLE_SO_VERSION**: Disable so version

You can also get them by

```shell
cd yajl
cmake -LH
```

Ubuntu/Debian
---------------------

```shell
sudo apt-get install -y cmake
cd yajl
cmake -G"Unix Makefiles"
make
```

Fedora/Red Hat/CentOS
---------------------

```shell
sudo yum install -y cmake
cd yajl
cmake -G"Unix Makefiles"
make
```

AIX
---------------------

Please get CMake RPM (http://www.perzl.org/aix/index.php?n=Main.Cmake)

```shell
cd yajl
cmake -G"Unix Makefiles"
make
```

Windows
---------------------

Please get CMake (http://www.cmake.org)

```shell
cmake -G"NMake Makefiles"
nmake
```
or

```shell
cmake -G"Visual Studio 10"
```


Packaging
=====================

Ubuntu/Debian
---------------------

```shell
sudo apt-get install -y build-essential cmake doxygen
cd yajl
dpkg-buildpackage -us -uc -rfakeroot
```

Fedora/Red Hat/CentOS
---------------------

```shell
sudo yum install -y cmake doxygen
cd yajl
git archive --prefix=yajl-2.0.5/ | bzip2 -9 > `rpm -E %{_topdir}`/yajl-2.0.5.tar.bz2
rpmbuild -ba yajl.spec
```

Additionally, you can build YAJL i686 rpm on x86_64 platform as

```shell
CC="gcc -m32" rpmbuild -ba --target=i686 yajl.spec
```

AIX
---------------------

Please get CMake RPM (http://www.perzl.org/aix/index.php?n=Main.Cmake)

```shell
cd yajl
git archive --prefix=yajl-2.0.5/ | bzip2 -9 > `rpm -E %{_topdir}`/yajl-2.0.5.tar.bz2
rpmbuild -ba yajl.spec
```

Additionally, you can build YAJL ppc64 rpm as

```shell
CC="gcc -maix64" rpmbuild -ba --target=ppc64 yajl.spec
```
