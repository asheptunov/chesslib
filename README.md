# Chess Engine

This is a bitboard-based chess state machine written (mostly) in C. The API is intended to be used for testing chess bots.

## Setup and Building from Source

### Build Requirements

A Unix shell

Git

CMake 3.15.3+

GNU Make 3.81+

GCC with support for C++11

flex 2.5.35+

### Build Instructions

To set up and build the engine, run

```shell
sh setup.sh
```

This will set up all build directories, install libraries and build binaries.

After running set up and making any changes, you can rebuild from source by running

```shell
make all
```

## Authors

Andriy Sheptunov

Neil Jurling

