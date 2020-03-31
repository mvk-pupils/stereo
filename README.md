
# Stereo

[![Build Status](https://travis-ci.org/mvk-pupils/stereo.svg?branch=master)](https://travis-ci.org/mvk-pupils/stereo)

## Building from Source

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```


### Disabling Doxygen

If you don't want to build the documentation run cmake with:

```
$ cmake .. -D BUILD_DOCUMENTATION=OFF
```


### Run executable

#### Windows

```
$ cd Debug
$ ./stereo-world.exe
```

#### Unix

```
./stereo-world
```


## Building the executable (Windows)

The executable depends on ICONIC Vision's proprietary library
which neeeds to be built beforehand. Refer to the instructions
in their documentation. 

1. Set the `ICONICPATH` environment variable to the path of 
   the library source.

2. Run cmake using `cmake <source_directory> -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake`
   where `source_directory` is the root of this repository and `<path_to_vcpkg>` 
   is the path to the vcpkg installation directory, eg: `C:/Users/<your_username>/vcpkg/scripts/buildsystems/vcpkg.cmake`

3. Build the solution using your preferred build system (eg. `cmake --build .`).

4. At this point there may be some DLLs missing from the build directory. 
   Copy these from where you built ICONIC's library next to the executable.
