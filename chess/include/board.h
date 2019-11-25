#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "defs.h"
#include "move.h"

using std::string;
using std::vector;

namespace game {

class Board {
   public:
    /**
     * Constructs a board from the given string, in Forsyth-Edwards notation. If no
     * string is specified, the starting position for chess is used.
     */
    Board(const char *fen = STARTING_BOARD);

    /**
     * Creates a board from another. The other board is deep copied, resulting in
     * two logically equal but physically independent boards.
     * 
     * @param other the other board
     */
    Board(const Board &other);

    /**
     * Destructs this board.
     */
    ~Board();

    /**
     * Assigns this board to the same state as another. The other board is deep
     * copied, resulting in two logically equal but physically independent boards,
     * as with
     * @link Board::Board(const &Board::Board) the copy constructor.
     * 
     * @param other the other board
     * @return *this
     */
    Board &operator=(const Board &other);

    /**
     * Applies the given move to this board. Assumes that the move was previously
     * returned by
     * @link Board::generateMoves generateMoves.
     * 
     * @param move the move to apply
     */
    void applyMove(const Move &move);

    /**
     * Generates and returns a vector of moves that the active player can make,
     * according to standard chess rules. If the active player is under checkmate,
     * returns an empty vector.
     * 
     * @return the vector of possible moves
     */
    vector<Move> generateMoves() const;

    /**
     * Returns the Forsyth-Edwards notation for this board.
     * 
     * @return the FEN
     */
    string toFen() const;

    /**
     * Prints a console representation of the board to the given output stream.
     * 
     * @param os the output stream to print to
     * @param board the board to print
     * @return the output stream
     */
    friend ostream &operator<<(ostream &os, const Board &board);

    /**
     * Returns true if the specified position is hit by any piece of the given
     * color, false otherwise. Takes rank in [0,7] corresponding to [1,8] and
     * offset in [0,7] corresponding to ['a','h']. Hit means direct hit, not
     * en passant takes. One should use this for seeing if a position is in check,
     * so en passant takes would be irrelevant.
     * 
     * @return true if the position is hit, false otherwise
     */
    bool hit(const int rk, const int offs, const bool white) const;

//    private:
    uint32_t ranks_[8];
    uint16_t flags_;

   private:
    void generatePawnMoves(vector<Move> *dest, const int rk, const int offs) const;
    void generateKnightMoves(vector<Move> *dest, const int rk, const int offs) const;
    void generateBishopMoves(vector<Move> *dest, const int rk, const int offs) const;
    void generateRookMoves(vector<Move> *dest, const int rk, const int offs) const;
    void generateQueenMoves(vector<Move> *dest, const int rk, const int offs) const;
    void generateKingMoves(vector<Move> *dest, const int rk, const int offs) const;
    bool _hitSingle(const int rk, const int offs, const bool white, uint8_t &blocks) const;
    bool _hitKnight(const int rk, const int offs, const bool white) const;
    bool _hitDiagonal(const int rk, const int offs, const bool white, uint8_t &blocks) const;
    bool _hitLateral(const int rk, const int offs, const bool white, uint8_t &blocks) const;
};

}  // namespace game
