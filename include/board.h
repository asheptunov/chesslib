#pragma once

#include "defs.h"
#include "move.h"
#include "arraylist.h"

/**
* A board with 8 ranks (ranks) and various flags (flags). See docs for details.
*/
typedef struct {
    uint32_t ranks[8];
    uint32_t flags;
} board_t;

/**
* Returns a board made from a Forsyth-Edwards Notation (FEN) string.
*/
board_t *board_make(const char *fen);

/**
* Returns a board deep copied from another board.
* 
*/
board_t *board_copy(const board_t *other);

/**
* Frees a board and all associated data.
*/
void board_free(const board_t *other);

/**
* Applies a move to the board. The move must be valid.
*/
#ifdef CHESSLIB_QWORD_MOVE
void board_apply_move(board_t *board, const move_t move);
#else
void board_apply_move(board_t *board, const move_t *move);
#endif

/**
* Returns an arraylist of all valid moves for the board.
*/
alst_t *board_get_moves(const board_t *board);

/**
* Returns 0 iff the current player is not under checkmate.
*/
int board_is_mate(const board_t *board);

/**
* Returns 0 iff the current player is not under stalemate or a draw by insufficient mating material.
* Does not currently account for draws by the fifty-move rule, by repitition, or by highly niche
* cases of insufficient mating material.
* Note: these games are rare, and will likely continue for an inordinate amount of moves. The client
* should keep track of more complex draw cases if they wish, but a simple approach to prune these games
* during game farming is to cut off games over a given turn limit (for example, 500 turns).
*/
int board_is_stalemate(const board_t *board);

/**
* Returns the Forsyth-Edwards Notation (FEN) for the board, excluding the halfmove clock and the fullmove
* number.
* Data in the returned buffer persists up to the next call.
*/
char *board_to_fen(const board_t *board);

/**
* Returns a TUI representation of the board.
* Data in the returned buffer persists up to the next call.
*/
char *board_to_tui(const board_t *board);

/**
* Returns 0 iff the specified position is not hit by any piece of the given color.
* Takes 0-indexed rank in [0, 7] corresponding to board ranks [1, 8].
* Takes 0-indexed offset in [0, 7] corresponding to board files [a, h].
* Searches only for direct hits, excluding en passant takes.
* Note: this should be used to detect if a position is in check, which is why
* en passant takes on the position are not considered.
*/
int _board_hit(const board_t *board, const int rk, const int offs, const int white);
