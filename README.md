# chesslib

This is a bitboard-based chess state machine API written (mostly) in C. The intended use is for hosting chess bots.

### Installation

There are three options for installing and building with this chess library.

#### Method 1 (Recommended). Loading the library with Python 3.7+ on a Unix system

Place `chess.py` and `pychess.ini` together in your project directory.

Point the `posix_path` option in `pychess.ini` to the fully-qualified path to `libchess.so`.

`import chess` and use as you would any other Python library.

#### Method 2. Loading the library with Python 3.7+ on a Windows system

Place `chess.py` and `pychess.ini` together in your project directory.

Point the `nt_path` option in `pychess.ini` to the fully-qualified path to `libchess.dll`.

`import chess` and use as you would any other Python library.

#### Method 3. Linking with the library using GCC

Place the contents of `include` to a project directory (`{include_dir}`).

Place `libchess.a` into a project directory (`{lib_dir}`).

Compile your C/C++ project files with GCC using `-I {include_dir}`.

Link your C/C++ project files with gcc using `-L {lib_dir} -lchess`.

`#include "defs.h"` and all other required headers and use as you would any other C library.

*Note: when linking with `libchess.a`, it is advisable to iron out memory leaks using a heap profiler like Valgrind,
as memory leaks can cause severe performance issues when generating huge amounts of board positions for bots.*

*Note: if the performance of your chess library is your bottleneck, linking with `libchess.a` is the recommended
option, and can often result in a tenfold performance increase in Perft over `chess.py`.*

---

### Building from source

#### Requirements

The preferred build method is using a Unix toolchain, but Windows recommendations are included should you require building on Windows.

- A Unix shell (MSYS2 on Windows)

- GCC (MinGW GCC on Windows)

- Make (MinGW Make on Windows)

- Flex

- Git, for fetching GTest libraries if you wish to build the c++ test suite (`make unittest`)

### Instructions

To initialize a (likely) working environment:

```shell
make init
```

For building a `.a` link-time library:

```shell
make clean liba
```

For building a Unix-style `.so` load-time library:

```shell
make clean libso
```

For building a Windows-style `.dll` load-time library (must be building with mingw gcc):

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

*Note: some of these tests may take a significant, hardware-dependent amount of time to complete, as they are exhaustive
search-based correctness tests for chess move generation.*

---

## Authors

Andriy Sheptunov

Neil Jurling
