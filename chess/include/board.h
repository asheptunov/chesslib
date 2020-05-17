#ifndef BOARD_H
#define BOARD_H

#include "defs.h"
#include "move.h"
#include "arraylist.h"

typedef struct {
    uint32_t ranks[8];
    uint32_t flags;
} board_t;

/**
     * Constructs a board from the given string, in Forsyth-Edwards notation. If no
     * string is specified, the starting position for chess is used.
     */
board_t *board_make(const char *fen);

/**
     * Creates a board from another. The other board is deep copied, resulting in
     * two logically equal but physically independent boards.
     * 
     * @param other the other board
     */
board_t *board_copy(const board_t *other);

/**
     * Destructs this board.
     */
void board_free(const board_t *other);

/**
     * Applies the given move to this board. Assumes that the move was previously
     * returned by
     * @link Board::generateMoves generateMoves.
     * 
     * @param move the move to apply
     */
void board_apply_move(board_t *board, const move_t move);

/**
     * Generates and returns a vector of moves that the active player can make,
     * according to standard chess rules. If the active player is under checkmate,
     * returns an empty vector.
     * 
     * @return the vector of possible moves
     */
alst_t board_get_moves(const board_t *board);

/**
     * Returns true if the game has ended in a mate, or specifically when the active player's king is in check and
     * no moves remain.
     *
     * @return true iff the game has ended in a mate
     */
int board_is_mate(const board_t *board);

/**
     * Returns true if the game has ended in a stalemate, false otherwise.
     *
     * @return true iff the game has ended in a stalemate
     */
int board_is_stalemate(const board_t *board);

/**
     * Returns the Forsyth-Edwards notation for this board.
     * 
     * @return the FEN
     */
char *board_to_fen(const board_t *board);

/**
     * Prints a console representation of the board to the given output stream.
     * 
     * @param os the output stream to print to
     * @param board the board to print
     * @return the output stream
     */
char *board_to_tui(const board_t *board);

#endif  // BOARD_H
