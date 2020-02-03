
# Stereo

[![Build Status](https://travis-ci.org/mvk-pupils/stereo.svg?branch=master)](https://travis-ci.org/mvk-pupils/stereo)

## Building from Source

```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

## Run code (Windows)

```
$ cd Debug
$ ./stereo-world.exe
```

## Run code (Unix)

```
./stereo-world.exe
```

### Disabling Doxygen

If you don't want to build the documentation run cmake with:

```
$ cmake .. -D BUILD_DOCUMENTATION=OFF
```

