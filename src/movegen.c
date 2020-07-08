#include "assert.h"
#include "stdarg.h"

#include "board.h"
#include "move.h"
#include "arraylist.h"

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
        attacker = ((board->ranks[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2) || attacker == (pc3) || attacker == (pc4)) { \
            return 1; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR3(rk, offs, pc1, pc2, pc3, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((board->ranks[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2) || attacker == (pc3)) { \
            return 1; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR2(rk, offs, pc1, pc2, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((board->ranks[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc1) || attacker == (pc2)) { \
            return 1; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

#define LOOKINGFOR1(rk, offs, pc, blocker) \
    if (ISPOS2((rk), (offs))) { \
        attacker = ((board->ranks[(rk)] >> (((offs)) * 4)) & 0xf); \
        if (attacker == (pc)) { \
            return 1; \
        } else if (attacker != NOPC) {  /* pc that can't hit us but blocks diagonal / lateral */ \
            blocker; \
        } \
    } else { \
        blocker; \
    } \

void _board_generatePawnMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
void _board_generateKnightMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
void _board_generateBishopMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
void _board_generateRookMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
void _board_generateQueenMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
void _board_generateKingMoves(const board_t *board, alst_t *dest, const int rk, const int offs);
int _board_hitSingle(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks);
int _board_hitKnight(const board_t *board, const int rk, const int offs, const int white);
int _board_hitDiagonal(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks);
int _board_hitLateral(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks);

typedef struct {
    int8_t dx;
    int8_t dy;
} _move_delta_t;

_move_delta_t knight_moves[8] = {{2 * UP, RT}, {2 * UP, LT}, {2 * DN, RT}, {2 * DN, LT}, {UP, 2 * RT}, {UP, 2 * LT}, {DN, 2 * RT}, {DN, 2 * LT}};
_move_delta_t bishop_moves[28] = {DIAG};
_move_delta_t rook_moves[28] = {LATERAL};
_move_delta_t queen_moves[56] = {DIAG, LATERAL};
_move_delta_t king_moves[8] = {{UP, 0}, {DN, 0}, {0, RT}, {0, LT}, {UP, RT}, {UP, LT}, {DN, RT}, {DN, LT}};

#define CUR_BISHOP_MOVE bishop_moves[i * 7 + j]
#define CUR_ROOK_MOVE rook_moves[i * 7 + j]
#define CUR_QUEEN_MOVE queen_moves[i * 7 + j]

alst_t *board_get_moves(const board_t *board) {
    alst_t *ret = alst_make(30);  // reserve 30

    pos_t kingpos = NOPOS;  // this should be set by the end, or we are in an invalid state
    const int player = FLAGS_BPLAYER(board->flags);  // 1 if current player is black, 0 if white
    // player represents parity of current player's pcs (pc / 6)

    uint32_t rank;
    int rk;
    int offs;
    int pc;

    // iterate A8 -> H1
    for (rk = 7; rk >= 0; --rk) {  // 8 to 1
        rank = board->ranks[rk];
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
                _board_generatePawnMoves(board, ret, rk, offs);
                break;
            case WKNIGHT:
            case BKNIGHT:
                _board_generateKnightMoves(board, ret, rk, offs);
                break;
            case WBISHOP:
            case BBISHOP:
                _board_generateBishopMoves(board, ret, rk, offs);
                break;
            case WROOK:
            case BROOK:
                _board_generateRookMoves(board, ret, rk, offs);
                break;
            case WQUEEN:
            case BQUEEN:
                _board_generateQueenMoves(board, ret, rk, offs);
                break;
            case WKING:
            case BKING:
                kingpos = POS2(offs, rk);
                _board_generateKingMoves(board, ret, rk, offs);
                break;
            default:
                break;
            }
            rank >>= 4;  // next pc in rank
        }
    }

    assert(kingpos != NOPOS);

    const pc_t king = FLAGS_BPLAYER(board->flags) ? BKING : WKING;
    size_t j = 0;  // end of kept portion
    for (size_t i = 0; i < ret->len; ++i) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t move = (move_t) alst_get(ret, i);
        board_t *board_future = board_copy(board);
        board_apply_move(board_future, move);
        pos_t kingpos_future = (MVFROMPC(move) == king) ? MVTOPOS(move) : kingpos;
        if (!_board_hit(board_future, kingpos_future / 8, kingpos_future % 8, player)) {  // king is not hit; keep
            // swap ith and jth move
            move_t tmp = (move_t) alst_get(ret, j);  // tmp = ar[j]
            alst_put(ret, j, (void *) move);            // ar[j] = ar[i]
            alst_put(ret, i, (void *) tmp);             // ar[i] = tmp
            ++j;
        } // else king is hit; forget this move (free)
        board_free(board_future);
#else
        move_t *move = (move_t *) alst_get(ret, i);
        board_t *board_future = board_copy(board);
        board_apply_move(board_future, move);
        pos_t kingpos_future = (move->frompc == king) ? move->topos : kingpos;
        if (!_board_hit(board_future, kingpos_future / 8, kingpos_future % 8, player)) {  // king is not hit; keep
            // swap ith and jth move
            move_t *tmp = (move_t *) alst_get(ret, j);  // tmp = ar[j]
            alst_put(ret, j, (void *) move);            // ar[j] = ar[i]
            alst_put(ret, i, (void *) tmp);             // ar[i] = tmp
            ++j;
        } else {  // king is hit; forget this move (free)
            move_free(move);
        }
        board_free(board_future);
#endif
    }
    ret->len = j;

    return ret;
}

int _board_hit(const board_t *board, const int rk, const int offs, const int white) {

    // since we are doing radius 1 checks disjoint from radius 2+ checks,
    // we need to make sure blocking pieces at rad1 block the corresponding
    // diagonal / lateral at rad2+. We keep track of this with the following
    // bit array, where bit 0 is UP, bit 1 is UR, bit 2 is RT, bit 3 is DR, etc.
    uint8_t blocks = 0;

    if (_board_hitSingle(board, rk, offs, white, &blocks)) {
        return 1;
    }
    if (_board_hitKnight(board, rk, offs, white)) {
        return 1;
    }
    if (_board_hitDiagonal(board, rk, offs, white, &blocks)) {
        return 1;
    }
    if (_board_hitLateral(board, rk, offs, white, &blocks)) {
        return 1;
    }
    return 0;
}

void _board_generatePawnMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {

    if (FLAGS_WPLAYER(board->flags)) {  // WHITE; moves go UP in rank

        // SINGLE AND DOUBLE UP MOVE (AND PROMOTION)
        if (ISPOS2(rk + 1, offs)
        && (((board->ranks[rk + 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk + 1 < 7) {
                // add single up move (WPAWN topc)
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION up moves
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WKNIGHT, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WBISHOP, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WROOK, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+1), NOPOS, WPAWN, WQUEEN, NOPC));
            }
            if (rk == 1
            && ISPOS2(rk + 2, offs)
            && (((board->ranks[rk + 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double up move
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk+2), NOPOS, WPAWN, WPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk + 1, offs - 1)) {
            pc_t killpc = (board->ranks[rk + 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up left diagonal take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up left diagonal promotion take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WKNIGHT, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WBISHOP, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WROOK, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk+1), POS2(offs-1, rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk + 1, offs + 1)) {
            pc_t killpc = (board->ranks[rk + 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc >= BPAWN && killpc <= BKING) {  // is black piece
                if (rk + 1 < 7) {
                    // add up right diagonal take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WPAWN, killpc));
                } else {  // killpos/topos is promotion
                    // add up right diagonal promotion take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WKNIGHT, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WBISHOP, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WROOK, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk+1), POS2(offs+1,rk+1), WPAWN, WQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (board->flags >> 8) & 0xff;
        if (eppos != NOPOS
        && ((ISPOS2(rk+1, offs-1) && eppos == POS2(offs - 1, rk + 1))
         || (ISPOS2(rk+1, offs+1) && eppos == POS2(offs + 1, rk + 1)))) {
            // add ep up take
            alst_append(dest, (void *) move_make(POS2(offs, rk), eppos, eppos - 8, WPAWN, WPAWN, BPAWN));
        }

    } else {  // BLACK; moves go DOWN in rank
        // WATCH SIGNING; IF rk-1<0 THIS SECTION COULD BREAK BY OVERFLOW

        // SINGLE AND DOUBLE DOWN MOVE (AND PROMOTION)
        if (ISPOS2(rk - 1, offs)
        && (((board->ranks[rk - 1] >> (offs * 4)) & 0xf) == NOPC)) {
            if (rk - 1 > 0) {
                // add single down move (BPAWN topc)
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BPAWN, NOPC));
            } else {  // topc is promotion
                // add PROMOTION down moves
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BKNIGHT, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BBISHOP, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BROOK, NOPC));
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-1), NOPOS, BPAWN, BQUEEN, NOPC));
            }
            if (rk == 6
            && ISPOS2(rk - 2, offs)
            && (((board->ranks[rk - 2] >> (offs * 4)) & 0xf) == NOPC)) {
                // add double down move
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs, rk-2), NOPOS, BPAWN, BPAWN, NOPC));
            }
        }

        // DIAGONAL TAKES (AND PROMOTION TAKES)
        if (ISPOS2(rk - 1, offs - 1)) {
            pc_t killpc = (board->ranks[rk - 1] >> ((offs - 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down left diagonal take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BKNIGHT, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BBISHOP, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BROOK, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-1, rk-1), POS2(offs-1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }
        if (ISPOS2(rk - 1, offs + 1)) {
            pc_t killpc = (board->ranks[rk - 1] >> ((offs + 1) * 4)) & 0xf;
            if (killpc <= WKING) {  // is white piece
                if (rk - 1 > 0) {
                    // add down right diagonal take
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BPAWN, killpc));
                } else {  // killpos/topos is promotion
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BKNIGHT, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BBISHOP, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BROOK, killpc));
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+1, rk-1), POS2(offs+1, rk-1), BPAWN, BQUEEN, killpc));
                }
            }
        }

        // EN PASSANT TAKES
        pos_t eppos = (board->flags >> 8) & 0xff;
        if (eppos != NOPOS
        && ((ISPOS2(rk-1, offs-1) && eppos == POS2(offs - 1, rk - 1))
         || (ISPOS2(rk-1, offs+1) && eppos == POS2(offs + 1, rk - 1)))) {
            // add ep down take
            alst_append(dest, (void *) move_make(POS2(offs, rk), eppos, eppos + 8, BPAWN, BPAWN, WPAWN));
        }
    }
}

void _board_generateKnightMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {
    const pc_t frompc = FLAGS_WPLAYER(board->flags) ? WKNIGHT : BKNIGHT;
    pc_t killpc;

    for (int i = 0; i < 8; ++i) {
        _move_delta_t mv = knight_moves[i];
        if (ISPOS2(rk + mv.dx, offs + mv.dy)) {  // in bounds
            killpc = (board->ranks[rk + mv.dx] >> ((offs + mv.dy) * 4)) & 0xf;
            if (killpc == NOPC) {
                // just a knight move
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+mv.dy, rk+mv.dx), NOPOS, frompc, frompc, NOPC));
            } else if ((killpc / 6) != (frompc / 6)) {  // pc is different color
                // knight capture
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+mv.dy, rk+mv.dx), POS2(offs+mv.dy, rk+mv.dx), frompc, frompc, killpc));
            }
        }
    }
}

void _board_generateBishopMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {
    const pc_t frompc = FLAGS_WPLAYER(board->flags) ? WBISHOP : BBISHOP;
    pc_t killpc;

    for (int i = 0; i < 4; ++i) {  // 4 diagonals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along diagonal
            if (ISPOS2(rk + CUR_BISHOP_MOVE.dx, offs + CUR_BISHOP_MOVE.dy)) {  // in bounds
                killpc = (board->ranks[rk+CUR_BISHOP_MOVE.dx] >> ((offs+CUR_BISHOP_MOVE.dy) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal bishop move
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_BISHOP_MOVE.dy, rk+CUR_BISHOP_MOVE.dx), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the diagonal
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_BISHOP_MOVE.dy, rk+CUR_BISHOP_MOVE.dx), POS2(offs+CUR_BISHOP_MOVE.dy, rk+CUR_BISHOP_MOVE.dx), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill diagonal if encountered another piece
                }
            } else {  // entire diagonal done when first hits bounds
                j = 7;  // kill the diagonal
            }
        }
    }
}

void _board_generateRookMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {
    const pc_t frompc = FLAGS_WPLAYER(board->flags) ? WROOK : BROOK;
    pc_t killpc;

    for (int i = 0; i < 4; ++i) {  // 4 laterals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along lateral
            if (ISPOS2(rk + CUR_ROOK_MOVE.dx, offs + CUR_ROOK_MOVE.dy)) {  // in bounds
                killpc = (board->ranks[rk+CUR_ROOK_MOVE.dx] >> ((offs+CUR_ROOK_MOVE.dy) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal rook move
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_ROOK_MOVE.dy, rk+CUR_ROOK_MOVE.dx), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the lateral
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_ROOK_MOVE.dy, rk+CUR_ROOK_MOVE.dx), POS2(offs+CUR_ROOK_MOVE.dy, rk+CUR_ROOK_MOVE.dx), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill lateral if encountered another piece
                }
            } else {  // entire lateral done when first hits bounds
                j = 7;  // kill the lateral
            }
        }
    }
}

void _board_generateQueenMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {
    const pc_t frompc = FLAGS_WPLAYER(board->flags) ? WQUEEN : BQUEEN;
    pc_t killpc;

    for (int i = 0; i < 8; ++i) {  // 8 laterals / diagonals
        for (int j = 0; j < 7; ++j) {  // 7 max possible moves along lat / diag
            if (ISPOS2(rk + CUR_QUEEN_MOVE.dx, offs + CUR_QUEEN_MOVE.dy)) {  // in bounds
                killpc = (board->ranks[rk+CUR_QUEEN_MOVE.dx] >> ((offs+CUR_QUEEN_MOVE.dy) * 4)) & 0xf;
                if (killpc == NOPC) {  // normal queen move
                    alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_QUEEN_MOVE.dy, rk+CUR_QUEEN_MOVE.dx), NOPOS, frompc, frompc, NOPC));
                } else {  // kill the lat / diag
                    if ((killpc / 6) != (frompc / 6)) {  // capture
                        alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+CUR_QUEEN_MOVE.dy, rk+CUR_QUEEN_MOVE.dx), POS2(offs+CUR_QUEEN_MOVE.dy, rk+CUR_QUEEN_MOVE.dx), frompc, frompc, killpc));
                    }
                    j = 7;  // unconditionally kill lat / diag if encountered another piece
                }
            } else {  // entire lat / diag done when first hits bounds
                j = 7;  // kill the lat / diag
            }
        }
    }
}

void _board_generateKingMoves(const board_t *board, alst_t *dest, const int rk, const int offs) {
    const pc_t frompc = FLAGS_WPLAYER(board->flags) ? WKING : BKING;
    pc_t killpc;

    // NORMAL KING MOVES
    for (int i = 0; i < 8; ++i) {
        _move_delta_t mv = king_moves[i];
        if (ISPOS2(rk + mv.dx, offs + mv.dy)) {  // in bounds
            killpc = (board->ranks[rk + mv.dx] >> ((offs + mv.dy) * 4)) & 0xf;
            if (killpc == NOPC) {
                // just a king move
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+mv.dy, rk+mv.dx), NOPOS, frompc, frompc, NOPC));
            } else if ((killpc / 6) != (frompc / 6)) {  // pc is different color
                // king capture
                alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+mv.dy, rk+mv.dx), POS2(offs+mv.dy, rk+mv.dx), frompc, frompc, killpc));
            }
        }
    }

    // CASTLING MOVES
    if (FLAGS_WPLAYER(board->flags)) {  // white

        if (FLAGS_WKCASTLE(board->flags)  // white kingside
        && (((board->ranks[rk] >> ((offs+1) * 4)) & 0xf) == NOPC)  // f1 empty
        && (((board->ranks[rk] >> ((offs+2) * 4)) & 0xf) == NOPC)  // g1 empty
        && !_board_hit(board, rk, offs, 0)       // king not in check
        && !_board_hit(board, rk, offs+1, 0)) {  // f1 not hit
//        && !hit(rk, offs+2, false)) {  // g1 not hit
            alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+2, rk), NOPOS, frompc, frompc, NOPC));
        }

        if (FLAGS_WQCASTLE(board->flags)  // white queenside
        && (((board->ranks[rk] >> ((offs-1) * 4)) & 0xf) == NOPC)  // d1 empty
        && (((board->ranks[rk] >> ((offs-2) * 4)) & 0xf) == NOPC)  // c1 empty
        && (((board->ranks[rk] >> ((offs-3) * 4)) & 0xf) == NOPC)  // b1 empty
        && !_board_hit(board, rk, offs, 0)       // king not in check
        && !_board_hit(board, rk, offs-1, 0)) {  // d1 not hit
//        && !hit(rk, offs-2, false)) {  // c1 not hit
            alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-2, rk), NOPOS, frompc, frompc, NOPC));
        }

    } else {  // black

        if (FLAGS_BKCASTLE(board->flags)  // black kingside
        && (((board->ranks[rk] >> ((offs+1) * 4)) & 0xf) == NOPC)  // f8 empty
        && (((board->ranks[rk] >> ((offs+2) * 4)) & 0xf) == NOPC)  // g8 empty
        && !_board_hit(board, rk, offs, 1)       // king not in check
        && !_board_hit(board, rk, offs+1, 1)) {  // f8 not hit
//        && !hit(rk, offs+2, true)) {  // g8 not hit
            alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs+2, rk), NOPOS, frompc, frompc, NOPC));
        }

        if (FLAGS_BQCASTLE(board->flags)  // black queenside
        && (((board->ranks[rk] >> ((offs-1) * 4)) & 0xf) == NOPC)  // d8 empty
        && (((board->ranks[rk] >> ((offs-2) * 4)) & 0xf) == NOPC)  // c8 empty
        && (((board->ranks[rk] >> ((offs-3) * 4)) & 0xf) == NOPC)  // b8 empty
        && !_board_hit(board, rk, offs, 1)       // king not in check
        && !_board_hit(board, rk, offs-1, 1)) {  // d8 not hit
//        && !hit(rk, offs-2, true)) {  // c8 not hit
            alst_append(dest, (void *) move_make(POS2(offs, rk), POS2(offs-2, rk), NOPOS, frompc, frompc, NOPC));
        }
    }
}

int _board_hitSingle(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks) {
    // switch between black and white pieces using the 6 constant offset
    const int rk_offs = (white) ? 1 : -1;
    const int pc_offs = (white) ? 0 : 6;

    pc_t attacker;

    // PAWNS AND OTHER SINGLE DIAGONAL ATTACKERS
    LOOKINGFOR4(rk-rk_offs, offs+1, WPAWN + pc_offs, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << (2+rk_offs)));
    LOOKINGFOR4(rk-rk_offs, offs-1, WPAWN + pc_offs, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << (6-rk_offs)));

    // NON-PAWN SINGLE DIAGONAL ATTACKERS
    LOOKINGFOR3(rk+rk_offs, offs+1, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << (2-rk_offs)));
    LOOKINGFOR3(rk+rk_offs, offs-1, WBISHOP + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << (6+rk_offs)));

    // SINGLE LATERAL ATTACKERS
    LOOKINGFOR3(rk+1, offs, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << 0));
    LOOKINGFOR3(rk, offs+1, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << 2));
    LOOKINGFOR3(rk-1, offs, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << 4));
    LOOKINGFOR3(rk, offs-1, WROOK + pc_offs, WQUEEN + pc_offs, WKING + pc_offs, *blocks |= (1 << 6));

    return 0;
}

int _board_hitKnight(const board_t *board, const int rk, const int offs, const int white) {
    const int pc_offs = (white) ? 0 : 6;

    pc_t attacker;

    // KNIGHT ATTACKERS
    for (int i = 0; i < 8; ++i) {
        _move_delta_t mv = knight_moves[i];
        LOOKINGFOR1(rk+mv.dx, offs+mv.dy, WKNIGHT + pc_offs, ;);
    }

    return 0;
}

int _board_hitDiagonal(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks) {
    const int pc_offs = (white) ? 0 : 6;
    pc_t attacker;

    // DIAGONAL ATTACKERS
    for (int i = 0; i < 4; ++i) {  // 4 diagonals
        if (*blocks & (1 << (i * 2 + 1))) {  // if single blocker from above, skip diagonal
            continue;
        }
        for (int j = 1; j < 7; ++j) {  // 7 squares per diagonal, exclude first one
            if (ISPOS2(rk+CUR_BISHOP_MOVE.dx, offs+CUR_BISHOP_MOVE.dy)) {
                attacker = ((board->ranks[rk+CUR_BISHOP_MOVE.dx]) >> ((offs+CUR_BISHOP_MOVE.dy) * 4)) & 0xf;
                if (attacker == WBISHOP + pc_offs || attacker == WQUEEN + pc_offs) {  // hit on diagonal
                    return 1;
                } else if (attacker != NOPC) {  // no hit but diagonal blocked; cont to next diagonal
                    break;
                }
            } else {  // ran out of bounds; continue to next diagonal
                break;
            }
        }
    }

    return 0;
}

int _board_hitLateral(const board_t *board, const int rk, const int offs, const int white, uint8_t *blocks) {
    const int pc_offs = (white) ? 0 : 6;
    pc_t attacker;

    // LATERAL ATTACKERS
    for (int i = 0; i < 4; ++i) {  // 4 laterals
        if (*blocks & (1 << (i * 2))) {  // if single blocker from above, skip lateral
            continue;
        }
        for (int j = 1; j < 7; ++j) {  // 7 squares per lateral, exclude first one
            if (ISPOS2(rk+CUR_ROOK_MOVE.dx, offs+CUR_ROOK_MOVE.dy)) {
                attacker = ((board->ranks[rk+CUR_ROOK_MOVE.dx]) >> ((offs+CUR_ROOK_MOVE.dy) * 4)) & 0xf;
                if (attacker == WROOK + pc_offs || attacker == WQUEEN + pc_offs) {  // hit on lateral
                    return 1;
                } else if (attacker != NOPC) {  // no hit but lateral blocked; cont to next lateral
                    break;
                }
            } else {  // ran out of bounds; continue to next lateral
                break;
            }
        }
    }

    return 0;
}
