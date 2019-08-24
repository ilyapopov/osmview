osmview
=======

Desktop [OpenStreetMap](https://openstreetmap.org) viewer.

Installation
------------

### Download

Source code is hosted [on Github](https://github.com/ilyapopov/osmview).

To clone the source repository you need [Git](https://git-scm.com/).
Install it on a apt based system using

    sudo apt install git

Then clone the source repository and update

    git clone https://github.com/ilyapopov/osmview

Then do:

    git submodule init && git submodule update

### Dependencies

- [libSDL2](https://www.libsdl.org/) with Image and TTF components.
- [libcurl](http://curl.haxx.se/)
- [boost](http://boost.org).filesystem
- [libSDL2pp](https://github.com/AMDmi3/libSDL2pp)
  is bundled within the repository.
  Shall be checked out automatically when cloning.

To install dependencies on apt-based system (Debian and Ubuntu):

    sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libcurl4-openssl-dev libboost-filesystem-dev

### Compile

To compile, you need a C++11 conforming compiler (GCC and Clang tested)
with a standard library, recent CMake.

    cd osmview
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build

Then run osmview:

    cd ..
    ./build/src/osmview

*Enjoy!*

Author
------

[Ilya Popov](https://github.com/ilyapopov)

License
-------

Source code available under terms of GPL v3 or later license.

Bundled libSDL2pp is under terms of zlib licence.

Map data [Copyright OpenStreetMap contributors](https://www.openstreetmap.org/copyright)
under [ODbL license](https://opendatacommons.org/licenses/odbl/).
