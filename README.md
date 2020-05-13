
# Stereo

[![Build Status](https://travis-ci.org/mvk-pupils/stereo.svg?branch=master)](https://travis-ci.org/mvk-pupils/stereo)

## Building the Library


### Dependencies

- Git
- OpenGL
- GLEW

> Note: Git is required to pull down other external dependencies such as OpenVR.
> See a commplete list below.

#### Dependencies fetched by Git

- GLFW
- OpenVR


#### Optional

- Doxygen


### Using CMake

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

> Note: if you are using vcpkg, remember to specify the `-DCMAKE_TOOLCHAIN_FILE`
> flag to point to your vcpkg bulid system [INSERT LINK HERE].


### Disabling Doxygen

If you don't want to build the documentation run cmake with:

```
$ cmake .. -D BUILD_DOCUMENTATION=OFF
```


## Building the example executable (Windows only)

The executable depends on ICONIC Vision's proprietary library which neeeds to be
built beforehand. Refer to the instructions in their documentation. 

1. Set the `ICONICPATH` environment variable to the path of the library source.

2. Run cmake using `cmake <source_directory>
   -DCMAKE_TOOLCHAIN_FILE=<path_to_vcpkg>/scripts/buildsystems/vcpkg.cmake`
   where `source_directory` is the root of this repository and `<path_to_vcpkg>`
   is the path to the vcpkg installation directory, eg:
   `C:/Users/<your_username>/vcpkg/scripts/buildsystems/vcpkg.cmake`

3. Build the solution using your preferred build system (eg. `cmake --build .`).

4. At this point there may be some DLLs missing from the build directory.  Copy
   these from where you built ICONIC's library next to the executable.
