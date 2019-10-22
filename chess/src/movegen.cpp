#include <cstdlib>
#include <vector>
#include <map>

#include "board.h"
#include "move.h"

using std::vector;
using std::map;

namespace game
{

#define UP (8)
#define RT (1)
#define DN (-8)
#define LT (-1)

#define UL (7)
#define UR (9)
#define DR (-7)
#define DL (-9)

#define DIAG \
    {UP, RT}, {UP * 2, RT * 2}, {UP * 3, RT * 3}, {UP * 4, RT * 4}, {UP * 5, RT * 5}, {UP * 6, RT * 6}, {UP * 7, RT * 7}, \
    {UP, LT}, {UP * 2, LT * 2}, {UP * 3, LT * 3}, {UP * 4, LT * 4}, {UP * 5, LT * 5}, {UP * 6, LT * 6}, {UP * 7, LT * 7}, \
    {DN, RT}, {DN * 2, RT * 2}, {DN * 3, RT * 3}, {DN * 4, RT * 4}, {DN * 5, RT * 5}, {DN * 6, RT * 6}, {DN * 7, RT * 7}, \
    {DN, LT}, {DN * 2, LT * 2}, {DN * 3, LT * 3}, {DN * 4, LT * 4}, {DN * 5, LT * 5}, {DN * 6, LT * 6}, {DN * 7, LT * 7}  \

#define LATERAL \
    {UP, 0}, {UP * 2, 0}, {UP * 3, 0}, {UP * 4, 0}, {UP * 5, 0}, {UP * 6, 0}, {UP * 7, 0}, \
    {DN, 0}, {DN * 2, 0}, {DN * 3, 0}, {DN * 4, 0}, {DN * 5, 0}, {DN * 6, 0}, {DN * 7, 0}, \
    {0, RT}, {0, 2 * RT}, {0, 3 * RT}, {0, 4 * RT}, {0, 5 * RT}, {0, 6 * RT}, {0, 7 * RT}, \
    {0, LT}, {0, 2 * LT}, {0, 3 * LT}, {0, 4 * LT}, {0, 5 * LT}, {0, 6 * LT}, {0, 7 * LT}  \

static map<int, vector<vector<int8_t>>> moves =
   {{WKNIGHT, {{2 * UP, RT}, {2 * UP, LT}, {2 * DN, RT}, {2 * DN, LT}, {UP, 2 * RT}, {UP, 2 * LT}, {DN, 2 * RT}, {DN, 2 * LT}}},
    {WBISHOP, {DIAG}},
    {WROOK,   {LATERAL}},
    {WQUEEN,  {DIAG, LATERAL}},
    {WKING,   {{UP, 0}, {DN, 0}, {0, RT}, {0, LT}, {UP, RT}, {UP, LT}, {DN, RT}, {DN, LT}}}};

vector<Move> Board::generateMoves() const {
    vector<Move> ret;

    pos_t kingpos = NOPOS;  // this should be set by the end, or we are in an invalid state

    uint32_t rank;
    // iterate A8 -> H1
    for (int rk = 7; rk >= 0; --rk) {  // 8 to 1
        rank = this->ranks_[rk];
        for (int offs = 0; offs < 8; ++offs) {  // a to h
            int pc = rank & 0xf;  // one nibble / pc at a time
            if ((pc / 6) != !(flags_ & PLAYER)) {  // immobile color; continue
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
                kingpos = POS(offs, rk);
                this->generateKingMoves(&ret, rk, offs);
                break;
            default:
                break;
            }
            rank >>= 4;  // next pc in rank
        }
    }

    assert(kingpos != NOPOS);

    return ret;
}

void Board::generatePawnMoves(vector<Move> *dest, int rk, int offs) const {

    if (FLAGS_WPLAYER(flags_)) {  // WHITE; moves go UP in rank

        // SINGLE AND DOUBLE UP MOVE (AND PROMOTION)
        if (ISPOS2(rk + 1, offs)
        && (((ranks_[rk + 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk + 1 < 7) {
                // add single up move (WPAWN topc)
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+1), NOPOS, WPAWN, WPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION up moves
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+1), NOPOS, WPAWN, WKNIGHT, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+1), NOPOS, WPAWN, WBISHOP, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+1), NOPOS, WPAWN, WROOK, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+1), NOPOS, WPAWN, WQUEEN, NOPC));
            }
            if (ISPOS2(rk + 2, offs)
            && (((ranks_[rk + 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double up move
                dest->push_back(Move(POS(offs, rk), POS(offs, rk+2), NOPOS, WPAWN, WPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk + 1, offs - 1)) {
            pc_t killpc = (ranks_[rk + 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up left diagonal take
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk+1), POS(offs-1, rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up left diagonal promotion take
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk+1), POS(offs-1, rk+1), WPAWN, WKNIGHT, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk+1), POS(offs-1, rk+1), WPAWN, WBISHOP, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk+1), POS(offs-1, rk+1), WPAWN, WROOK, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk+1), POS(offs-1, rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk + 1, offs + 1)) {
            pc_t killpc = (ranks_[rk + 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up right diagonal take
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk+1), POS(offs+1,rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up right diagonal promotion take
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk+1), POS(offs+1,rk+1), WPAWN, WKNIGHT, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk+1), POS(offs+1,rk+1), WPAWN, WBISHOP, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk+1), POS(offs+1,rk+1), WPAWN, WROOK, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk+1), POS(offs+1,rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (flags_ >> 8) & 0xff;
        if (eppos != NOPOS
        && (eppos == POS(offs - 1, rk + 1) || eppos == POS(offs + 1, rk + 1))) {
            // add ep up take
            dest->push_back(Move(POS(offs, rk), eppos, eppos - 8, WPAWN, WPAWN, BPAWN));
        }

    } else {  // BLACK; moves go DOWN in rank
        // WATCH SIGNING; IF rk-1<0 THIS SECTION COULD BREAK BY OVERFLOW

        // SINGLE AND DOUBLE DOWN MOVE (AND PROMOTION)
        if (ISPOS2(rk - 1, offs)
        && (((ranks_[rk - 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk - 1 > 0) {
                // add single down move (BPAWN topc)
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-1), NOPOS, BPAWN, BPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION down moves
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-1), NOPOS, BPAWN, BKNIGHT, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-1), NOPOS, BPAWN, BBISHOP, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-1), NOPOS, BPAWN, BROOK, NOPC));
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-1), NOPOS, BPAWN, BQUEEN, NOPC));
            }
            if (ISPOS2(rk - 2, offs)
            && (((ranks_[rk - 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double down move
                dest->push_back(Move(POS(offs, rk), POS(offs, rk-2), NOPOS, BPAWN, BPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk - 1, offs - 1)) {
            pc_t killpc = (ranks_[rk - 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down left diagonal take
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk-1), POS(offs-1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk-1), POS(offs-1, rk-1), BPAWN, BKNIGHT, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk-1), POS(offs-1, rk-1), BPAWN, BBISHOP, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk-1), POS(offs-1, rk-1), BPAWN, BROOK, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs-1, rk-1), POS(offs-1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk - 1, offs + 1)) {
            pc_t killpc = (ranks_[rk - 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down right diagonal take
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk-1), POS(offs+1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk-1), POS(offs+1, rk-1), BPAWN, BKNIGHT, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk-1), POS(offs+1, rk-1), BPAWN, BBISHOP, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk-1), POS(offs+1, rk-1), BPAWN, BROOK, killpc));
                    dest->push_back(Move(POS(offs, rk), POS(offs+1, rk-1), POS(offs+1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (flags_ >> 8) & 0xff;
        if (eppos != NOPOS
        && (eppos == POS(offs - 1, rk - 1) || eppos == POS(offs + 1, rk - 1))) {
            // add ep down take
            dest->push_back(Move(POS(offs, rk), eppos, eppos + 8, BPAWN, BPAWN, WPAWN));
        }
    }
}

void Board::generateKnightMoves(vector<Move> *dest, int rk, int offs) const {
    pc_t killpc;
    pc_t frompc = FLAGS_WPLAYER(flags_) ? WKNIGHT : BKNIGHT;

    vector<vector<int8_t>> &knightMoves = moves[WKNIGHT];
    for (const vector<int8_t> &mv : knightMoves) {
        if (ISPOS2(rk + mv[0], offs + mv[1])) {  // legal move
            killpc = (ranks_[rk + mv[0]] >> ((offs + mv[1]) * 4)) & 0xff;
            if (killpc == NOPC) {
                // just a knight move
                dest->push_back(Move(POS(offs, rk), POS(offs+mv[1], rk+mv[0]), NOPOS, frompc, frompc, NOPC));
            } else if ((killpc / 6) != (frompc / 6)) {  // pc is different color
                // knight capture
                dest->push_back(Move(POS(offs, rk), POS(offs+mv[1], rk+mv[0]), POS(offs+mv[1], rk+mv[0]), frompc, frompc, killpc));
            }
        }
    }
}

void Board::generateBishopMoves(vector<Move> *dest, int rk, int offs) const {
    // check all diagonals, stop checking particular diagonal when hit another piece or spill
}

void Board::generateRookMoves(vector<Move> *dest, int rk, int offs) const {
    // check all laterals, stop checking particular lateral when hit another piece or spill
}

void Board::generateQueenMoves(vector<Move> *dest, int rk, int offs) const {
    // check both diagonals and laterals, stop any one when hit piece or spill
}

void Board::generateKingMoves(vector<Move> *dest, int rk, int offs) const {
    // check all immediate moves/takes
    // check castling
}

} // namespace game
