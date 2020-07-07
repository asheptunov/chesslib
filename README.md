# chesslib

This is a bitboard-based chess state machine API written (mostly) in C. The intended use is for hosting chess bots.

### Installation

There are three options for installing and building with this chess library.

#### Loading the library with Python 3.7+ on a Unix system
Place `chess.py` and `pychess.ini` together in your project directory, and point the `posix_path` option in
`pychess.ini` to the fully-qualified path to `libchess.so`. Import and use `chess` as you would any other
Python library.

#### Loading the library with Python 3.7+ on a Windows system
Place `chess.py` and `pychess.ini` together in your project directory, and point the `nt_path` option in
`pychess.ini` to the fully-qualified path to `libchess.dll`. Import and use `chess` as you would any other
Python library.

#### Linking with the library using GCC
Unpack the contents of `include` to your project directory (`{include_dir}`), and `libchess.a` to your project
directory (`{lib_dir}`). Compile your C/C++ project files with GCC using `-I {include_dir}`. LInk your C/C++
project files with gcc using `-L {lib_dir} -lchess`.

### Build Requirements

The preferred build method is using a Unix toolchain, but Windows alternative recommendations are parenthesized.

- A Unix shell (MSYS2 on Windows)

- GCC (MinGW GCC on Windows)

- Make (MinGW Make on Windows)

- Flex

- Git, for fetching GTest libraries if you wish to build the c++ test suite (`make unittest`)

### Build Instructions

For building a `.a` link-time library:

```shell
make clean liba
```

For building a Unix-style `.so` load-time library:

```shell
make clean libso
```

For building Windows-style `.dll` load-time library (must be building with mingw gcc):

```shell
make clean libdll
```

For building and running C++ unit test suites:

```shell
make clean unittest
```

For building and running end-to-end (integration) test suites (requires Python 3.7+ and Valgrind):

```shell
make clean systemtest
```

## Authors

Andriy Sheptunov

Neil Jurling
