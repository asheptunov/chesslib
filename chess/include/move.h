#pragma once

#include <cstdint>
#include <iostream>
#include <string>

#include "defs.h"

namespace game {

using std::ostream;
using std::string;

class Move {
   public:
    pos_t frompos_;
    pos_t topos_;
    pos_t killpos_;
    pc_t frompc_;
    pc_t topc_;
    pc_t killpc_;

    /**
     * Constructs a move with the given positions and pieces for the origin,
     * destination and kill target.
     * 
     * @param frompos the origin position
     * @param topos the destination position
     * @param killpos the position of the piece being killed, or NOPOS if this
     * is not a capture
     * @param frompc the origin piece
     * @param topc the destination piece, which equals frompc unless this move
     * is a promotion
     * @param killpc the piece being killed, or NOPC if this is not a capture
     */
    Move(const pos_t frompos,
         const pos_t topos,
         const pos_t killpos,
         const pc_t frompc,
         const pc_t topc,
         const pc_t killpc) : frompos_(frompos),
                              topos_(topos),
                              killpos_(killpos),
                              frompc_(frompc),
                              topc_(topc),
                              killpc_(killpc) {};

    /**
     * Constructs a move from its verbose algebraic notation. The notation
     * should be in the form "<pc><frompos><topos>" for normal moves, the form
     * "<frompc><frompos><'x'><killpc><topos>" for captures, and the form
     * "<frompc><frompos><'x'><killpc><topos><'e.p.'>" for en passant captures.
     * Promotions should have <topc> as the last token.
     */
    Move(const char *algNot);

    Move() : Move(NOPOS, NOPOS, NOPOS, NOPC, NOPC, NOPC) {};

    /**
     * Constructs a move from another move. This will result in two logically
     * equivalent but physically independent moves.
     */
    Move(const Move &other) : Move(other.frompos_, other.topos_, other.killpos_, other.frompc_, other.topc_, other.killpc_) {}

    /**
     * Destructs this move.
     */
    ~Move();

    /**
     * Assigns this move to the same state as the other move. This move will
     * have the same state as the other but be physically independent.
     */
    Move &operator=(const Move &other);

    /**
     * Returns true if this move is a capture, false otherwise.
     * 
     * @return true if this move is a capture, false otherwise
     */
    bool isCapture() const;

    /**
     * Returns true if this move is an en passant capture, false otherwise.
     * 
     * @return true if this move is an en passant capture, false otherwise.
     */
    bool isEnPassant() const;

    /**
     * Returns true if this move is a promotion, false otherwise.
     * 
     * @return true if this move is a promotion, false otherwise
     */
    bool isPromotion() const;

    /**
     * Returns the castling type of this move, if it is a castling move.
     * Otherwise returns 0. Castling moves will be one of WKCASTLE, WQCASTLE,
     * BKCASTLE and BQCASTLE.
     * Note that a castling move has a king as the frompc, rather than a rook.
     * 
     * @return the castling type of this move, or 0 if it is not a catling move
     */
    int castleType() const;

    /**
     * Returns true if this move is less than the other, false otherwise.
     * Comparison goes in order of frompc, frompos, topc, topos, killpos, and
     * finally topos.
     * 
     * @return true if this move is less than the other, false otherwise
     */
    bool operator<(const Move &other) const;

    /**
     * Returns the algebraic notation for this move.
     * 
     * @return the algebraic notation for this move
     */
    string algNot() const;

    /**
     * Prints a console representation of the given move to the given output
     * stream.
     * 
     * @param os the output stream to print to
     * @param move the move to print
     * @return the output stream
     */
    friend ostream &operator<<(ostream &os, const Move &move);
};

}  // namespace game
