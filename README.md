osmview
=======

Desktop [OpenStreetMap](https://openstreetmap.org) viewer.

Usage
=====


Installation
============

Download
--------
Source code is hosted [on Bitbucket](https://bitbucket.org/ipopov/osmview).

To clone the source repository you need
[Mercurial](https://www.mercurial-scm.org/) and [Git](https://git-scm.com/).
Install them on a apt based system using

    sudo apt install mercurial git

Then clone the source repository and update

    hg clone http://bitbucket.org/ipopov/osmview

Dependencies
------------

- [libSDL2](https://www.libsdl.org/) with Image and TTF components.
- [libcurl](http://curl.haxx.se/)
- [boost](http://boost.org).filesystem
- [libSDL2pp](https://github.com/AMDmi3/libSDL2pp)
  is bundled within the repository.
  Shall be checked out automatically when cloning.

To install depemdencies on apt-besed system (Debian and Ubuntu):

    sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libcurl4-openssl-dev libboost-filesystem-dev

Compile
-------

To compile, you need a C++11 conforming compiler (GCC and Clang tested)
with a standard library, recent CMake.

    cd osmview
    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make

Then run osmview:

    ./build/src/osmview

*Enjoy!*

Author
======

Ilya Popov <ilya.b.popov@gmail.com>

Links
=====

Source code is hosted [on Bitbucket](https://bitbucket.org/ipopov/osmview).
Feel free to provide feedback, file bugs and make pull requests there.

License
=======

Source code available under terms of GPL v3 or later license.

Bundled libSDL2pp is under terms of zlib licence.

Map data [Copyright OpenStreetMap contributors](https://www.openstreetmap.org/copyright)
under [ODbL license](http://www.opendatacommons.org/licenses/odbl).
