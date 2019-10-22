# Board Design

## Files:
include/board.h
src/board.cpp

## Overview

The board is composed of an array of 8 32 bit integers representing the 8 ranks,
and a 16 bit integer for flags and bookkeeping. A detailed breakdown follows below.

## Ranks

The `ranks_` array is indexed from 0-7, corresponding to board ranks 1-8, respectively.
A single element of `ranks_` represents the 8 files A-H in a single rank, in
LSB -> MSB order each file is a nibble. For example, file 1 is `ranks_[?] & 0xf`,
file 2 is `ranks_[?] & (0xf << 4)`, etc. Square B6, for example, can be read
using `(ranks_[1] >> (5 * 4)) & 0xf`; index 1 is for rank B (`\'b\' - \'a\'`) and
5 is `6 - 1`, since files are indexed from 0-7. `5 * 4` is used since the 6th file
is 5 nibbles up from the least significant nibble.

The values stored at the nibbles represent pieces. They are used as follows.
0x0 = white pawn
0x1 = white knight
0x2 = white bishop
0x3 = white rook
0x4 = white queen
0x5 = white king
0x6 = black pawn
0x7 = black knight
0x8 = black bishop
0x9 = black rook
0xa = black queen
0xb = black king
0xc = no piece

## Flags

The flags array contains various bookkeeping information about the game state. It
is prone to expansion. A detailed breakdown of each bit follows below.

Bit 1: 1 iff white kingside castling is legal
Bit 2: 1 iff white queenside castling is legal
Bit 3: 1 iff black kingside castling is legal
Bit 4: 1 iff black queenside castling is legal
Bit 5: 1 iff the active player is white
Bits 9-16: the en passant position, or NOPOS if no ep square was exposed last turn
