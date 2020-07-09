#pragma once

#include <stdint.h>

#include "defs.h"

/**
* A move by a piece (frompc) with a starting square (frompos), an ending square (topos),
* an optional victim piece (killpc) on square (killpos), and an optionally different piece
* identifier (topc) if the move was a promotion (whereby a pawn switches to a knight, bishop,
* rook, or queen).
* (topos) and (killpos) are the same for normal captures, differ for en passant captures
* (the position the pawn moves *to* differs from the position of the killed pawn *killpc*).
* (killpos) is NOPOS if the move is not a capture.
* (killpc) is NOPC if the move is not a capture.
*/
#ifdef CHESSLIB_QWORD_MOVE
typedef uint64_t move_t;
#define MVFROMPOS(move) ((pos_t) ((move) & 0xff))
#define MVTOPOS(move)   ((pos_t) (((move) >> 8) & 0xff))
#define MVKILLPOS(move) ((pos_t) (((move) >> 16) & 0xff))
#define MVFROMPC(move)  ((pc_t)  (((move) >> 24) & 0xff))
#define MVTOPC(move)    ((pc_t)  (((move) >> 32) & 0xff))
#define MVKILLPC(move)  ((pc_t)  (((move) >> 40) & 0xff))
#define MVMAKE(frompos, topos, killpos, frompc, topc, killpc) ((((((0L | ((uint64_t) (frompos))) | (((uint64_t) (topos)) << 8)) | (((uint64_t) (killpos)) << 16)) | (((uint64_t) (frompc)) << 24)) | (((uint64_t) (topc)) << 32)) | (((uint64_t) (killpc)) << 40))
#else
typedef struct {
    pos_t frompos;  // 8b
    pos_t topos;    // 8b
    pos_t killpos;  // 8b
    pc_t frompc;  // 8b
    pc_t topc;    // 8b
    pc_t killpc;  // 8b
} move_t;  // 6*8=48b -> uint64_t
#endif

/**
* Returns a move made from values.
*/
#ifdef CHESSLIB_QWORD_MOVE
move_t move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc);
#else
move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc);
#endif

/**
* Returns a move made from an (extended) algebraic notation string.
* The notation must include fully qualified position and piece strings, i.e., 'Re1xke5' rather than 'Rxe5'.
* En passant captures must end in 'e.p.'., i.e., 'pc4xPb3e.p.' rather than 'exb3'.
* Castling moves must be in terms of the move made by the king, i.e., 'Ke1c1' rather than '0-0-0'.
* Promotions must be followed by the destination (promoted-to) piece, i.e., 'Pf7f8Q'.
*/
#ifdef CHESSLIB_QWORD_MOVE
move_t move_make_algnot(const char *algnot);
#else
move_t *move_make_algnot(const char *algnot);
#endif

/**
* Returns a deep copy of another move.
*/
#ifndef CHESSLIB_QWORD_MOVE
move_t *move_cpy(move_t *other);
#endif

/**
* Frees a move and all associated data.
*/
#ifndef CHESSLIB_QWORD_MOVE
void move_free(move_t *move);
#endif

/**
* Returns the result of comparision of moves a and b.
* The result is negative if a < b, positive if a > b, and 0 if a equals b.
* Note: operators < and > don't order moves in any standard way, and should only
* be used for sorting arrays of moves. The intended use of this method is to compare
* two moves for equality.
*/
#ifdef CHESSLIB_QWORD_MOVE
int move_cmp(const move_t a, const move_t b);
#else
int move_cmp(const move_t *a, const move_t *b);
#endif

/**
* Returns 0 iff the move is not a capture.
*/
#ifdef CHESSLIB_QWORD_MOVE
int move_is_cap(const move_t move);
#else
int move_is_cap(const move_t *move);
#endif

/**
* Returns 0 iff the move is not an en passant capture.
*/
#ifdef CHESSLIB_QWORD_MOVE
int move_is_ep(const move_t move);
#else
int move_is_ep(const move_t *move);
#endif

/**
* Returns 0 iff the move is not a promotion.
*/
#ifdef CHESSLIB_QWORD_MOVE
int move_is_promo(const move_t move);
#else
int move_is_promo(const move_t *move);
#endif

/**
* Returns 0 iff the move is not a castling move.
*/
#ifdef CHESSLIB_QWORD_MOVE
int move_is_castle(const move_t move);
#else
int move_is_castle(const move_t *move);
#endif

/**
* Returns the (lossy) algebraic notation for the move.
* Data in the returned buffer persists up to the next call.
* Note: this move string is a compressed (lossy) notation for the move,
* and cannot be used to recover a move. Should you need to serialize a move,
* consider using its raw bytes, or printing it in lossless algebraic notation
* using move_algnot(move).
*/
#ifdef CHESSLIB_QWORD_MOVE
char *move_str(const move_t move);
#else
char *move_str(const move_t *move);
#endif

/**
* Returns the (lossless) algebraic notation for the move. The move can be
* fully recovered from this notation using move_make_algnot(move_algnot(move)).
* Data in the returned buffer persists up to the next call.
*/
#ifdef CHESSLIB_QWORD_MOVE
char *move_algnot(const move_t move);
#else
char *move_algnot(const move_t *move);
#endif
