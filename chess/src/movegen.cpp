#include <cstdlib>
#include <vector>
#include <map>

#include "board.h"
#include "move.h"

using std::vector;
using std::map;

namespace game
{

#define UP (1)
#define RT (1)
#define DN (-1)
#define LT (-1)

#define DIAG \
    {UP, RT}, {UP * 2, RT * 2}, {UP * 3, RT * 3}, {UP * 4, RT * 4}, {UP * 5, RT * 5}, {UP * 6, RT * 6}, {UP * 7, RT * 7}, \
    {DN, RT}, {DN * 2, RT * 2}, {DN * 3, RT * 3}, {DN * 4, RT * 4}, {DN * 5, RT * 5}, {DN * 6, RT * 6}, {DN * 7, RT * 7}, \
    {DN, LT}, {DN * 2, LT * 2}, {DN * 3, LT * 3}, {DN * 4, LT * 4}, {DN * 5, LT * 5}, {DN * 6, LT * 6}, {DN * 7, LT * 7}, \
    {UP, LT}, {UP * 2, LT * 2}, {UP * 3, LT * 3}, {UP * 4, LT * 4}, {UP * 5, LT * 5}, {UP * 6, LT * 6}, {UP * 7, LT * 7}  \

#define LATERAL \
    {UP, 0}, {UP * 2, 0}, {UP * 3, 0}, {UP * 4, 0}, {UP * 5, 0}, {UP * 6, 0}, {UP * 7, 0}, \
    {0, RT}, {0, 2 * RT}, {0, 3 * RT}, {0, 4 * RT}, {0, 5 * RT}, {0, 6 * RT}, {0, 7 * RT}, \
    {DN, 0}, {DN * 2, 0}, {DN * 3, 0}, {DN * 4, 0}, {DN * 5, 0}, {DN * 6, 0}, {DN * 7, 0}, \
    {0, LT}, {0, 2 * LT}, {0, 3 * LT}, {0, 4 * LT}, {0, 5 * LT}, {0, 6 * LT}, {0, 7 * LT}  \

#define LOOKINGFOR4(rk, offs, pc1, pc2, pc3, pc4, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((ranks_[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2) || attacker == (pc3) || attacker == (pc4)) { \
            return true; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR3(rk, offs, pc1, pc2, pc3, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((ranks_[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2) || attacker == (pc3)) { \
            return true; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR2(rk, offs, pc1, pc2, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((ranks_[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2)) { \
            return true; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR1(rk, offs, pc, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((ranks_[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc)) { \
            return true; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

static const vector<vector<int8_t>> knightMoves =
        {{2 * UP, RT}, {2 * UP, LT}, {2 * DN, RT}, {2 * DN, LT}, {UP, 2 * RT}, {UP, 2 * LT}, {DN, 2 * RT}, {DN, 2 * LT}};
static const vector<vector<int8_t>> bishopMoves =
        {DIAG};
static const vector<vector<int8_t>> rookMoves =
        {LATERAL};
static const vector<vector<int8_t>> queenMoves =
        {DIAG, LATERAL};
static const vector<vector<int8_t>> kingMoves =
        {{UP, 0}, {DN, 0}, {0, RT}, {0, LT}, {UP, RT}, {UP, LT}, {DN, RT}, {DN, LT}};

#define CUR_BISHOP_MOVE bishopMoves[i * 7 + j]
#define CUR_ROOK_MOVE rookMoves[i * 7 + j]
#define CUR_QUEEN_MOVE queenMoves[i * 7 + j]

vector<Move> Board::generateMoves() const {
    vector<Move> ret;
    ret.reserve(30);

    pos_t kingpos = NOPOS;  // this should be set by the end, or we are in an invalid state
    const bool player = FLAGS_BPLAYER(flags_);  // 1 if current player is black, 0 if white
    // player represents parity of current player's pcs (pc / 6)

    uint32_t rank;
    int rk;
    int offs;
    int pc;

    // iterate A8 -> H1
    for (rk = 7; rk >= 0; --rk) {  // 8 to 1
        rank = this->ranks_[rk];
        for (offs = 0; offs < 8; ++offs) {  // a to h
            pc = rank & 0xf;  // one nibble / pc at a time
            if (player ^ (pc >= BPAWN)) {  // piece doesn't match active player
                rank >>= 4;
                continue;
            }
            switch (pc) {
            case NOPC:
                break;
            case WPAWN:
            case BPAWN:
                this->generatePawnMoves(&ret, rk, offs);
                break;
            case WKNIGHT:
            case BKNIGHT:
                this->generateKnightMoves(&ret, rk, offs);
                break;
            case WBISHOP:
            case BBISHOP:
                this->generateBishopMoves(&ret, rk, offs);
                break;
            case WROOK:
            case BROOK:
                this->generateRookMoves(&ret, rk, offs);
                break;
            case WQUEEN:
            case BQUEEN:
                this->generateQueenMoves(&ret, rk, offs);
                break;
            case WKING:
            case BKING:
                kingpos = POS2(offs, rk);
                this->generateKingMoves(&ret, rk, offs);
                break;
            default:
                break;
            }
            rank >>= 4;  // next pc in rank
        }
    }

    assert(kingpos != NOPOS);

    const pc_t king = FLAGS_BPLAYER(flags_) ? BKING : WKING;

    auto end = std::remove_if(ret.begin(), ret.end(), [this, player, king, kingpos](const Move &mv) {
        Board fut(*this);  // reset to current
        fut.applyMove(mv);  // next future
        pos_t kingpos2 = (mv.frompc_ == king) ? mv.topos_ : kingpos;
        return fut.hit(kingpos2 / 8, kingpos2 % 8, player);  // remove if king in check
    });

    ret.erase(end, ret.end());
    return ret;
}

void Board::_generatePawnMoves(vector<Move> *dest, const int rk, const int offs) const {

    if (FLAGS_WPLAYER(flags_)) {  // WHITE; moves go UP in rank

        // SINGLE AND DOUBLE UP MOVE (AND PROMOTION)
        if (ISPOS2(rk + 1, offs)
        && (((ranks_[rk + 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk + 1 < 7) {
                // add single up move (WPAWN topc)
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION up moves
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WKNIGHT, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WBISHOP, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WROOK, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WQUEEN, NOPC));
            }
            if (rk == 1
            && ISPOS2(rk + 2, offs)
            && (((ranks_[rk + 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double up move
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk+2), NOPOS, WPAWN, WPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk + 1, offs - 1)) {
            pc_t killpc = (ranks_[rk + 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up left diagonal take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up left diagonal promotion take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WKNIGHT, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WBISHOP, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WROOK, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk + 1, offs + 1)) {
            pc_t killpc = (ranks_[rk + 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up right diagonal take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up right diagonal promotion take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WKNIGHT, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WBISHOP, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WROOK, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (flags_ >> 8) & 0xff;
        if (eppos != NOPOS
        && ((ISPOS2(rk+1, offs-1) && eppos == POS2(offs - 1, rk + 1))
         || (ISPOS2(rk+1, offs+1) && eppos == POS2(offs + 1, rk + 1)))) {
            // add ep up take
            dest->push_back(Move(POS2(offs, rk), eppos, eppos - 8, WPAWN, WPAWN, BPAWN));
        }

    } else {  // BLACK; moves go DOWN in rank
        // WATCH SIGNING; IF rk-1<0 THIS SECTION COULD BREAK BY OVERFLOW

        // SINGLE AND DOUBLE DOWN MOVE (AND PROMOTION)
        if (ISPOS2(rk - 1, offs)
        && (((ranks_[rk - 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk - 1 > 0) {
                // add single down move (BPAWN topc)
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION down moves
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BKNIGHT, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BBISHOP, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BROOK, NOPC));
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BQUEEN, NOPC));
            }
            if (rk == 6
            && ISPOS2(rk - 2, offs)
            && (((ranks_[rk - 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double down move
                dest->push_back(Move(POS2(offs, rk), POS2(offs, rk-2), NOPOS, BPAWN, BPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk - 1, offs - 1)) {
            pc_t killpc = (ranks_[rk - 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down left diagonal take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BKNIGHT, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BBISHOP, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BROOK, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk - 1, offs + 1)) {
            pc_t killpc = (ranks_[rk - 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down right diagonal take
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BKNIGHT, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BBISHOP, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BROOK, killpc));
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (flags_ >> 8) & 0xff;
        if (eppos != NOPOS
        && ((ISPOS2(rk-1, offs-1) && eppos == POS2(offs - 1, rk - 1))
         || (ISPOS2(rk-1, offs+1) && eppos == POS2(offs + 1, rk - 1)))) {
            // add ep down take
            dest->push_back(Move(POS2(offs, rk), eppos, eppos + 8, BPAWN, BPAWN, WPAWN));
        }
    }
}

void Board::_generateKnightMoves(vector<Move> *dest, const int rk, const int offs) const {
    const pc_t frompc = FLAGS_WPLAYER(flags_) ? WKNIGHT : BKNIGHT;
    pc_t killpc;

    for (const vector<int8_t> &mv : knightMoves) {
        if (ISPOS2(rk + mv[0], offs + mv[1])) {  // in bounds
            killpc = (ranks_[rk + mv[0]] >> ((offs + mv[1]) * 4)) & 0xf;
            if (killpc == NOPC) {
                // just a knight move
                dest->push_back(Move(POS2(offs, rk), POS2(offs+mv[1], rk+mv[0]), NOPOS, frompc, frompc, NOPC));
            } else if ((killpc / 6) != (frompc / 6)) {  // pc is different color
                // knight capture
                dest->push_back(Move(POS2(offs, rk), POS2(offs+mv[1], rk+mv[0]), POS2(offs+mv[1], rk+mv[0]), frompc, frompc, killpc));
            }
        }
    }
}

void Board::_generateBishopMoves(vector<Move> *dest, const int rk, const int offs) const {
    const pc_t frompc = FLAGS_WPLAYER(flags_) ? WBISHOP : BBISHOP;
    pc_t killpc;

    for (int i = 0; i < 4; ++i) {  // 4 diagonals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along diagonal
            if (ISPOS2(rk + CUR_BISHOP_MOVE[0], offs + CUR_BISHOP_MOVE[1])) {  // in bounds
                killpc = (ranks_[rk+CUR_BISHOP_MOVE[0]] >> ((offs+CUR_BISHOP_MOVE[1]) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal bishop move
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_BISHOP_MOVE[1], rk+CUR_BISHOP_MOVE[0]), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the diagonal
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_BISHOP_MOVE[1], rk+CUR_BISHOP_MOVE[0]), POS2(offs+CUR_BISHOP_MOVE[1], rk+CUR_BISHOP_MOVE[0]), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill diagonal if encountered another piece
                }
            } else {  // entire diagonal done when first hits bounds
                j = 7;  // kill the diagonal
            }
        }
    }
}

void Board::_generateRookMoves(vector<Move> *dest, const int rk, const int offs) const {
    const pc_t frompc = FLAGS_WPLAYER(flags_) ? WROOK : BROOK;
    pc_t killpc;

    for (int i = 0; i < 4; ++i) {  // 4 laterals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along lateral
            if (ISPOS2(rk + CUR_ROOK_MOVE[0], offs + CUR_ROOK_MOVE[1])) {  // in bounds
                killpc = (ranks_[rk+CUR_ROOK_MOVE[0]] >> ((offs+CUR_ROOK_MOVE[1]) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal rook move
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_ROOK_MOVE[1], rk+CUR_ROOK_MOVE[0]), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the lateral
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_ROOK_MOVE[1], rk+CUR_ROOK_MOVE[0]), POS2(offs+CUR_ROOK_MOVE[1], rk+CUR_ROOK_MOVE[0]), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill lateral if encountered another piece
                }
            } else {  // entire lateral done when first hits bounds
                j = 7;  // kill the lateral
            }
        }
    }
}

void Board::_generateQueenMoves(vector<Move> *dest, const int rk, const int offs) const {
    const pc_t frompc = FLAGS_WPLAYER(flags_) ? WQUEEN : BQUEEN;
    pc_t killpc;

    for (int i = 0; i < 8; ++i) {  // 8 laterals / diagonals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along lat / diag
            if (ISPOS2(rk + CUR_QUEEN_MOVE[0], offs + CUR_QUEEN_MOVE[1])) {  // in bounds
                killpc = (ranks_[rk+CUR_QUEEN_MOVE[0]] >> ((offs+CUR_QUEEN_MOVE[1]) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal queen move
                    dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_QUEEN_MOVE[1], rk+CUR_QUEEN_MOVE[0]), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the lat / diag
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        dest->push_back(Move(POS2(offs, rk), POS2(offs+CUR_QUEEN_MOVE[1], rk+CUR_QUEEN_MOVE[0]), POS2(offs+CUR_QUEEN_MOVE[1], rk+CUR_QUEEN_MOVE[0]), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill lat / diag if encountered another piece
                }
            } else {  // entire lat / diag done when first hits bounds
                j = 7;  // kill the lat / diag
            }
        }
    }
}

void Board::_generateKingMoves(vector<Move> *dest, const int rk, const int offs) const {
    const pc_t frompc = FLAGS_WPLAYER(flags_) ? WKING : BKING;
    pc_t killpc;

    // NORMAL KING MOVES
    for (const vector<int8_t> &mv : kingMoves) {
        if (ISPOS2(rk + mv[0], offs + mv[1])) {  // in bounds
            killpc = (ranks_[rk + mv[0]] >> ((offs + mv[1]) * 4)) & 0xf;
            if (killpc == NOPC) {
                // just a king move
                dest->push_back(Move(POS2(offs, rk), POS2(offs+mv[1], rk+mv[0]), NOPOS, frompc, frompc, NOPC));
            } else if ((killpc / 6) != (frompc / 6)) {  // pc is different color
                // king capture
                dest->push_back(Move(POS2(offs, rk), POS2(offs+mv[1], rk+mv[0]), POS2(offs+mv[1], rk+mv[0]), frompc, frompc, killpc));
            }
        }
    }

    // CASTLING MOVES
    if (FLAGS_WPLAYER(flags_)) {  // white

        if (FLAGS_WKCASTLE(flags_)  // white kingside
        && (((ranks_[rk] >> ((offs+1) * 4)) & 0xf) == NOPC)  // f1 empty
        && (((ranks_[rk] >> ((offs+2) * 4)) & 0xf) == NOPC)  // g1 empty
        && !hit(rk, offs, false)       // king not in check
        && !hit(rk, offs+1, false)) {  // f1 not hit
//        && !hit(rk, offs+2, false)) {  // g1 not hit
            dest->push_back(Move(POS2(offs, rk), POS2(offs+2, rk), NOPOS, frompc, frompc, NOPC));
        }

        if (FLAGS_WQCASTLE(flags_)  // white queenside
        && (((ranks_[rk] >> ((offs-1) * 4)) & 0xf) == NOPC)  // d1 empty
        && (((ranks_[rk] >> ((offs-2) * 4)) & 0xf) == NOPC)  // c1 empty
        && (((ranks_[rk] >> ((offs-3) * 4)) & 0xf) == NOPC)  // b1 empty
        && !hit(rk, offs, false)       // king not in check
        && !hit(rk, offs-1, false)) {  // d1 not hit
//        && !hit(rk, offs-2, false)) {  // c1 not hit
            dest->push_back(Move(POS2(offs, rk), POS2(offs-2, rk), NOPOS, frompc, frompc, NOPC));
        }

    } else {  // black

        if (FLAGS_BKCASTLE(flags_)  // black kingside
        && (((ranks_[rk] >> ((offs+1) * 4)) & 0xf) == NOPC)  // f8 empty
        && (((ranks_[rk] >> ((offs+2) * 4)) & 0xf) == NOPC)  // g8 empty
        && !hit(rk, offs, true)       // king not in check
        && !hit(rk, offs+1, true)) {  // f8 not hit
//        && !hit(rk, offs+2, true)) {  // g8 not hit
            dest->push_back(Move(POS2(offs, rk), POS2(offs+2, rk), NOPOS, frompc, frompc, NOPC));
        }

        if (FLAGS_BQCASTLE(flags_)  // black queenside
        && (((ranks_[rk] >> ((offs-1) * 4)) & 0xf) == NOPC)  // d8 empty
        && (((ranks_[rk] >> ((offs-2) * 4)) & 0xf) == NOPC)  // c8 empty
        && (((ranks_[rk] >> ((offs-3) * 4)) & 0xf) == NOPC)  // b8 empty
        && !hit(rk, offs, true)       // king not in check
        && !hit(rk, offs-1, true)) {  // d8 not hit
//        && !hit(rk, offs-2, true)) {  // c8 not hit
            dest->push_back(Move(POS2(offs, rk), POS2(offs-2, rk), NOPOS, frompc, frompc, NOPC));
        }
    }
}

bool Board::hit(const int rk, const int offs, const bool white) const {

    // since we are doing radius 1 checks disjoint from radius 2+ checks,
    // we need to make sure blocking pieces at rad1 block the corresponding
    // diagonal / lateral at rad2+. We keep track of this with the following
    // bit array, where bit 0 is UP, bit 1 is UR, bit 2 is RT, bit 3 is DR, etc.
    uint8_t blocks = 0;

    if (_hitSingle(rk, offs, white, blocks)) {
        return true;
    }

    if (_hitKnight(rk, offs, white)) {
        return true;
    }

    if (_hitDiagonal(rk, offs, white, blocks)) {
        return true;
    }

    if (_hitLateral(rk, offs, white, blocks)) {
        return true;
    }

    return false;
}

bool Board::_hitSingle(const int rk, const int offs, const bool white, uint8_t &blocks) const {
    // switch between black and white pieces using the 6 constant offset
    const int rk_offs = (white) ? 1 : -1;
    const int pc_offs = (white) ? 0 : 6;

    pc_t attacker;

    // PAWNS AND OTHER SINGLE DIAGONAL ATTACKERS
    LOOKINGFOR4(rk-rk_offs, offs+1, WPAWN + pc_offs, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << (2+rk_offs)));
    LOOKINGFOR4(rk-rk_offs, offs-1, WPAWN + pc_offs, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << (6-rk_offs)));

    // NON-PAWN SINGLE DIAGONAL ATTACKERS
    LOOKINGFOR3(rk+rk_offs, offs+1, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << (2-rk_offs)));
    LOOKINGFOR3(rk+rk_offs, offs-1, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << (6+rk_offs)));

    // SINGLE LATERAL ATTACKERS
    LOOKINGFOR3(rk+1, offs, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << 0));
    LOOKINGFOR3(rk, offs+1, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << 2));
    LOOKINGFOR3(rk-1, offs, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << 4));
    LOOKINGFOR3(rk, offs-1, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, blocks |= (1 << 6));

    return false;
}

bool Board::_hitKnight(const int rk, const int offs, const bool white) const {
    const int pc_offs = (white) ? 0 : 6;

    pc_t attacker;

    // KNIGHT ATTACKERS
    for (const vector<int8_t> &mv : knightMoves) {
        LOOKINGFOR1(rk+mv[0], offs+mv[1], WKNIGHT + pc_offs, ;);
    }

    return false;
}

bool Board::_hitDiagonal(const int rk, const int offs, const bool white, uint8_t &blocks) const {
    const int pc_offs = (white) ? 0 : 6;
    pc_t attacker;

    // DIAGONAL ATTACKERS
    for (int i = 0; i < 4; ++i) {  // 4 diagonals
        if (blocks & (1 << (i * 2 + 1))) {  // if single blocker from above, skip diagonal
            continue;
        }
        for (int j = 1; j < 7; ++j) {  // 7 squares per diagonal, exclude first one
            if (ISPOS2(rk+CUR_BISHOP_MOVE[0], offs+CUR_BISHOP_MOVE[1])) {
                attacker = ((ranks_[rk+CUR_BISHOP_MOVE[0]]) >> ((offs+CUR_BISHOP_MOVE[1]) * 4)) & 0xf;
                if (attacker == WBISHOP + pc_offs || attacker == WQUEEN + pc_offs) {  // hit on diagonal
                    return true;
                } else if (attacker != NOPC) {  // no hit but diagonal blocked; cont to next diagonal
                    break;
                }
            } else {  // ran out of bounds; continue to next diagonal
                break;
            }
        }
    }

    return false;
}

bool Board::_hitLateral(const int rk, const int offs, const bool white, uint8_t &blocks) const {
    const int pc_offs = (white) ? 0 : 6;
    pc_t attacker;

    // LATERAL ATTACKERS
    for (int i = 0; i < 4; ++i) {  // 4 laterals
        if (blocks & (1 << (i * 2))) {  // if single blocker from above, skip lateral
            continue;
        }
        for (int j = 1; j < 7; ++j) {  // 7 squares per lateral, exclude first one
            if (ISPOS2(rk+CUR_ROOK_MOVE[0], offs+CUR_ROOK_MOVE[1])) {
                attacker = ((ranks_[rk+CUR_ROOK_MOVE[0]]) >> ((offs+CUR_ROOK_MOVE[1]) * 4)) & 0xf;
                if (attacker == WROOK + pc_offs || attacker == WQUEEN + pc_offs) {  // hit on lateral
                    return true;
                } else if (attacker != NOPC) {  // no hit but lateral blocked; cont to next lateral
                    break;
                }
            } else {  // ran out of bounds; continue to next lateral
                break;
            }
        }
    }

    return false;
}

} // namespace game
