# Chess Engine

This is a bitboard-based chess state machine written (mostly) in C. The API is intended to be used for testing chess bots.

## Setup and Building from Source

### Build Requirements

A Unix shell

Git

CMake 3.15.3+

GNU Make 3.81+

GCC with support for C99 and C++11

flex 2.5.35+

### Build Instructions

First, run

```shell
make init
```

Unit test binaries can be run using

```shell
make unittest
```

System (end-to-end) tests can be run using

```shell
make systemtest
```

To build from source, use

```shell
make clean all
```

## Authors

Andriy Sheptunov

Neil Jurling

