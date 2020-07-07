#pragma once

#include "defs.h"

/**
* A move by a piece (frompc) with a starting position (frompos), an ending position (topos),
* an optional victim piece (killpc) on position (killpos), and an optionally different piece
* identifier (topc) if the move was a promotion (whereby a pawn switches to a knight, bishop,
* rook, or queen).
* (topos) and (killpos) are the same for normal captures, differ for en passant captures
* (the position the pawn moves *to* differs from the position of the killed pawn *killpc*).
* (killpos) is NOPOS if the move is not a capture.
* (killpc) is NOPC if the move is not a capture.
*/
typedef struct {
    pos_t frompos;
    pos_t topos;
    pos_t killpos;
    pc_t frompc;
    pc_t topc;
    pc_t killpc;
} move_t;

/**
* Returns a move made from values.
*/
move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc);

/**
* Returns a move made from an (extended) algebraic notation string.
* The notation must include fully qualified position and piece strings, i.e., 'Re1xke5' rather than 'Rxe5'.
* En passant captures must end in 'e.p.'., i.e., 'pc4xPb3e.p.' rather than 'exb3'.
* Castling moves must be in terms of the move made by the king, i.e., 'Ke1c1' rather than '0-0-0'.
* Promotions must be followed by the destination (promoted-to) piece, i.e., 'Pf7f8Q'.
*/
move_t *move_make_algnot(const char *algnot);

/**
* Returns a deep copy of another move.
*/
move_t *move_cpy(move_t *other);

/**
* Frees a move and all associated data.
*/
void move_free(move_t *move);

/**
* Returns the result of comparision of moves a and b.
* The result is negative if a < b, positive if a > b, and 0 if a equals b.
* Note: operators < and > don't order moves in any standard way, and should only
* be used for sorting arrays of moves. The intended use of this method is to compare
* two moves for equality.
*/
int move_cmp(const move_t *a, const move_t *b);

/**
* Returns 0 iff the move is not a capture.
*/
int move_is_cap(const move_t *move);

/**
* Returns 0 iff the move is not an en passant capture.
*/
int move_is_ep(const move_t *move);

/**
* Returns 0 iff the move is not a promotion.
*/
int move_is_promo(const move_t *move);

/**
* Returns 0 iff the move is not a castling move.
*/
int move_is_castle(const move_t *move);

/**
* Returns the (extended) algebraic notation for the move. The move
* can be fully restored (duplicated) using move_make_algnot(move_str(move)).
* Data in the returned buffer persists up to the next call.
*/
char *move_str(const move_t *move);
