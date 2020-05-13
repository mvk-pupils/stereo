
# Stereo

[![Build Status](https://travis-ci.org/mvk-pupils/stereo.svg?branch=master)](https://travis-ci.org/mvk-pupils/stereo)

## Building the Library


### Dependencies

- OpenGL
- GLEW
- OpenVR


#### Optional (for documentation and example executable)

- Doxygen
- GLFW
- I-CONIC Vision Video Decoder (proprietary)


### Using CMake

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

> Note: if you are using vcpkg, remember to specify the `-DCMAKE_TOOLCHAIN_FILE`
> flag to point to your [vcpkg bulid system](https://github.com/Microsoft/vcpkg#quick-start).


## Building Documentation with Doxygen

You can build the documentation by running

```
$ mkdir build
$ cd build
$ cmake .. -D BUILD_DOCUMENTATION=ON
```


## Building the example executable (Windows only)

The executable depends on ICONIC Vision's proprietary library which neeeds to be
built beforehand. Refer to the instructions in their documentation. 

Set the `ICONICPATH` environment variable to the path of the library source for
ICONIC's library.

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --target stereo-world
```
