# JARTSy

This repository contains the source code of [JARTSy][], which may or may not
become a half decent ray tracing program :-) 

[JARTSy]: https://github.com/ruevs/jartsy


## What is This

This is "homework" for the [Chaos Camp][chaos] [Ray Tracing Course 2022][rtc].

[chaos]: https://www.chaos.com/chaos-camp
[rtc]: https://www.chaos.com/chaos-camp/ray-tracing-course-2022

## Example Usage

Render `myscene.jrts` into `myscene.ppm` in 1024x768 resolution.

```sh
jartsy render -o %.ppm --size 1024x768 myscene.jrts
```

## Building on Linux

You will need the usual build tools, CMake. On a Debian derivative (e.g.
Ubuntu) these can be installed with:

```sh
sudo apt install git build-essential cmake
```

On a RedHat derivative (e.g. Fedora) the dependencies can be installed with:

```sh
sudo dnf install git gcc-c++ cmake
```

Before building, check out the project and the necessary submodules:

```sh
git clone https://github.com/ruevs/jartsy
cd jartsy
#not needed for now
#git submodule update --init
```

After that, build JARTSy like this:

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=ON
make
```

Link Time Optimization is supported by adding `-DENABLE_LTO=ON` to cmake at the
expense of longer build time.


## Building on macOS

You will need git, XCode tools, CMake and libomp. Git, CMake and libomp can be
installed via [Homebrew][]:

```sh
brew install git cmake libomp
```

XCode has to be installed via AppStore or [the Apple website][appledeveloper];
it requires a free Apple ID.

Before building, check out the project and the necessary submodules:

```sh
git clone https://github.com/ruevs/jartsy
cd jartsy
#not needed for now
#git submodule update --init
```

After that, build JARTSy like this:

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENMP=ON
make
```

Link Time Optimization is supported by adding `-DENABLE_LTO=ON` to cmake at the
expense of longer build time.

Alternatively, generate an XCode project, open it, and build the "Release"
scheme:

```sh
mkdir build
cd build
cmake .. -G Xcode
```

[homebrew]: https://brew.sh/
[appledeveloper]: https://developer.apple.com/download/

## Building on Windows

You will need [git][gitwin], [cmake][cmakewin] and a C++ compiler
(either Visual C++ or MinGW). If using Visual C++, Visual Studio 2015
or later is required.

### Check Out the Project

```bat
git clone https://github.com/ruevs/jartsy
cd jartsy
#rem not needed for now
#rem submodule update --init
```

### Building with Visual Studio IDE

Create a directory `build` in
the source tree and point cmake-gui to the source tree and that directory.
Press "Configure" and "Generate", then open `build\jartsy.sln` with
Visual C++ and build it.

Or simply use the CMake support integrated in modern Visual Studio by choosing
File | Open | Folder... and hitting `F7`.

### Building with Visual Studio in a command prompt

First, ensure that `git` and `cl` (the Visual C++ compiler driver) are in your
`%PATH%`; the latter is usually done by invoking `vcvarsall.bat` from your
Visual Studio install. Then, run the following in cmd or PowerShell:

```bat
mkdir build
cd build
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
```

### Building with MinGW

It is also possible to build JARTSy using [MinGW][mingw].

First, ensure that git and gcc are in your `$PATH`. Then, run the following
in bash:

```sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

[gitwin]: https://git-scm.com/download/win
[cmakewin]: http://www.cmake.org/download/#latest
[mingw]: http://www.mingw.org/

## License

JARTSy is distributed under the terms of the [GPL v3](COPYING.txt) or later.
