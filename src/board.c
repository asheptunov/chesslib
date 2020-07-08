#include <stdlib.h>
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <stdio.h>

#include "board.h"
#include "parseutils.h"

board_t *board_make(const char *fen) {
    board_t *ret = (board_t *) calloc(1, sizeof(board_t));
    if (!ret) {
        fprintf(stderr, "malloc error in board_make\n");
        exit(EXIT_FAILURE);
    }

    char fencpy[82];  // 8*8 (board) + 7 (slashes) + 3 (spaces) + 1 (player) + 4 (rights) + 2 (ep) + 1 (\0)
    // char *fencpy = strdup(fen);  // mutability
#ifdef __STDC_LIB_EXT1__
    size_t strmax = sizeof fencpy;
    const char *delim = " ";
    char *next_token;
    strcpy_s(fencpy, strmax, fen);  // mutability
    char *board_p = strtok_s(fencpy, &strmax, delim, &next_token);  // split off board data
    char *player_p = strtok_s(NULL, &strmax, delim, &next_token);  // split off player data
    char *castling_p = strtok_s(NULL, &strmax, delim, &next_token);  // split off castling data
    char *ep_p = strtok_s(NULL, &strmax, delim, &next_token);  // split off en passant data
    const char *fenranks[8];
    strmax = 72;  // 8*8 (board) + 7 (slashes) + 1 (\0)
    delim = "/";
    fenranks[0] = strtok_s(board_p, &strmax, delim, &next_token);  // split board data by ranks
    for (int i = 1; i < 8; ++i) {
        fenranks[i] = strtok_s(NULL, &strmax, delim, &next_token);
    }
#else
    const char *delim = " ";
    strcpy(fencpy, fen);  // mutability
    char *board_p = strtok(fencpy, delim);  // split off board data
    char *player_p = strtok(NULL, delim);  // split off player data
    char *castling_p = strtok(NULL, delim);  // split off castling data
    char *ep_p = strtok(NULL, delim);  // split off en passant data
    const char *fenranks[8];
    delim = "/";
    fenranks[0] = strtok(board_p, delim);  // split board data by ranks
    for (int i = 1; i < 8; ++i) {
        fenranks[i] = strtok(NULL, delim);
    }
#endif
    // build each rank from the fen rank data
    int offs, pc;
    for (int rk = 8; rk > 0; --rk) {
        ret->ranks[rk - 1] = 0xcccccccc;  // init to NOPC
        offs = 0;
#ifdef __STDC_LIB_EXT1__
        for (unsigned int i = 0; i < strlen_s(fenranks[8 - rk], 9); ++i) {  // 9 (max) = 8 (rank) + 1 (\0)
#else
        for (unsigned int i = 0; i < strlen(fenranks[8 - rk]); ++i) {
#endif
            pc = piece_from_char(fenranks[8 - rk][i]);

            // store king position, if a king is in the rank
            if (pc == WKING) {
                SETWKING(POS2(offs, rk-1), ret->flags);
            } else if (pc == BKING) {
                SETBKING(POS2(offs, rk-1), ret->flags);
            }

            ZEROPOS(offs, ret->ranks[rk - 1]);
            SETPOS(offs, ret->ranks[rk - 1], pc);
            offs += (pc == NOPC) ? fenranks[8 - rk][i] - '0' : 1;
        }
    }

    // set player bits from fen player data
    SETPLAYER((strcmp("w", player_p) ? BPLAYER : WPLAYER), ret->flags);

    // set castling bits from fen castling data
    ZEROCASTLE(ret->flags);
    if (strchr(castling_p, 'K')) SETCASTLE(WKCASTLE, ret->flags);
    if (strchr(castling_p, 'Q')) SETCASTLE(WQCASTLE, ret->flags);
    if (strchr(castling_p, 'k')) SETCASTLE(BKCASTLE, ret->flags);
    if (strchr(castling_p, 'q')) SETCASTLE(BQCASTLE, ret->flags);

    // set en passant bits and position from fen ep data
    SETEP(strchr(ep_p, '-') ? NOPOS : POS(ep_p[0], ep_p[1] - '0'), ret->flags);

    // free(fencpy);
    return ret;
}

board_t *board_copy(const board_t *other) {
    board_t *ret = (board_t *) malloc(sizeof(board_t));
    if (!ret) {
        fprintf(stderr, "malloc error in board_copy\n");
        exit(EXIT_FAILURE);
    }
    memcpy(&ret->ranks, &other->ranks, sizeof(int32_t) * 8);  // copy ranks
    ret->flags = other->flags;  // copy flags
    return ret;
}

void board_free(const board_t *other) {
    free((void *) other);
}

#ifdef CHESSLIB_QWORD_MOVE
void board_apply_move(board_t *board, const move_t move) {
#else
void board_apply_move(board_t *board, const move_t *move) {
#endif
    // kill the target piece if the move is a capture
    if (move_is_cap(move)) {
#ifdef CHESSLIB_QWORD_MOVE
        int k_rk = MVKILLPOS(move) / 8;
        int k_offs = MVKILLPOS(move) % 8;
#else
        int k_rk = move->killpos / 8;
        int k_offs = move->killpos % 8;
#endif
        ZEROPOS(k_offs, board->ranks[k_rk]);
        SETPOS(k_offs, board->ranks[k_rk], NOPC);

        // update castling rights / bits if killed piece was an opponent's rook that could've castled
#ifdef CHESSLIB_QWORD_MOVE
        if (MVKILLPC(move) == WROOK) {
            if (MVKILLPOS(move) == POS('a', 1)) {
                ZEROCASTLE2(board->flags, WQCASTLE);
            } else if (MVKILLPOS(move) == POS('h', 1)) {
                ZEROCASTLE2(board->flags, WKCASTLE);
            }
        } else if (MVKILLPC(move) == BROOK) {
            if (MVKILLPOS(move) == POS('a', 8)) {
                ZEROCASTLE2(board->flags, BQCASTLE);
            } else if (MVKILLPOS(move) == POS('h', 8)) {
                ZEROCASTLE2(board->flags, BKCASTLE);
            }
        }
#else
        if (move->killpc == WROOK) {
            if (move->killpos == POS('a', 1)) {
                ZEROCASTLE2(board->flags, WQCASTLE);
            } else if (move->killpos == POS('h', 1)) {
                ZEROCASTLE2(board->flags, WKCASTLE);
            }
        } else if (move->killpc == BROOK) {
            if (move->killpos == POS('a', 8)) {
                ZEROCASTLE2(board->flags, BQCASTLE);
            } else if (move->killpos == POS('h', 8)) {
                ZEROCASTLE2(board->flags, BKCASTLE);
            }
        }
#endif
    }

    // move the piece
#ifdef CHESSLIB_QWORD_MOVE
    int f_rk = MVFROMPOS(move) / 8;
    int t_rk = MVTOPOS(move) / 8;
    int f_offs = MVFROMPOS(move) % 8;
    int t_offs = MVTOPOS(move) % 8;
    MOVEPC(f_offs, t_offs, board->ranks[f_rk], board->ranks[t_rk], MVTOPC(move));
#else
    int f_rk = move->frompos / 8;
    int t_rk = move->topos / 8;
    int f_offs = move->frompos % 8;
    int t_offs = move->topos % 8;
    MOVEPC(f_offs, t_offs, board->ranks[f_rk], board->ranks[t_rk], move->topc);
#endif

    // also move the rook if castling
    switch (move_is_castle(move)) {
        case 0: break;
        case WKCASTLE:
            MOVEPC2('h', 'f', board->ranks[0], board->ranks[0], WROOK); break;
        case WQCASTLE:
            MOVEPC2('a', 'd', board->ranks[0], board->ranks[0], WROOK); break;
        case BKCASTLE:
            MOVEPC2('h', 'f', board->ranks[7], board->ranks[7], BROOK); break;
        case BQCASTLE:
            MOVEPC2('a', 'd', board->ranks[7], board->ranks[7], BROOK); break;
    }

    // update castling rights / bits if castled or made a move that voids castling
#ifdef CHESSLIB_QWORD_MOVE
    switch (MVFROMPC(move)) {
        case WROOK: {
            if (MVFROMPOS(move) == POS('a', 1)) {
                ZEROCASTLE2(board->flags, WQCASTLE);
            } else if (MVFROMPOS(move) == POS('h', 1)) {
                ZEROCASTLE2(board->flags, WKCASTLE);
            }
            break;
        }
        case WKING:
            ZEROCASTLE2(board->flags, WKCASTLE);
            ZEROCASTLE2(board->flags, WQCASTLE);
            break;
        case BROOK: {
            if (MVFROMPOS(move) == POS('a', 8)) {
                ZEROCASTLE2(board->flags, BQCASTLE);
            } else if (MVFROMPOS(move) == POS('h', 8)) {
                ZEROCASTLE2(board->flags, BKCASTLE);
            }
            break;
        }
        case BKING:
            ZEROCASTLE2(board->flags, BKCASTLE);
            ZEROCASTLE2(board->flags, BQCASTLE);
            break;
    }
#else
    switch (move->frompc) {
        case WROOK: {
            if (move->frompos == POS('a', 1)) {
                ZEROCASTLE2(board->flags, WQCASTLE);
            } else if (move->frompos == POS('h', 1)) {
                ZEROCASTLE2(board->flags, WKCASTLE);
            }
            break;
        }
        case WKING:
            ZEROCASTLE2(board->flags, WKCASTLE);
            ZEROCASTLE2(board->flags, WQCASTLE);
            break;
        case BROOK: {
            if (move->frompos == POS('a', 8)) {
                ZEROCASTLE2(board->flags, BQCASTLE);
            } else if (move->frompos == POS('h', 8)) {
                ZEROCASTLE2(board->flags, BKCASTLE);
            }
            break;
        }
        case BKING:
            ZEROCASTLE2(board->flags, BKCASTLE);
            ZEROCASTLE2(board->flags, BQCASTLE);
            break;
    }
#endif

    // disable en passant rights / bits unless made a move that allows en passant
    SETEP(NOPOS, board->flags);
#ifdef CHESSLIB_QWORD_MOVE
    if ((MVFROMPC(move) == WPAWN || MVFROMPC(move) == BPAWN) \
        && (abs(MVTOPOS(move) - MVFROMPOS(move)) == 16)) {
        SETEP((MVFROMPOS(move) + MVTOPOS(move)) / 2, board->flags);
    }
#else
    if ((move->frompc == WPAWN || move->frompc == BPAWN) \
        && (abs(move->topos - move->frompos) == 16)) {
        SETEP((move->frompos + move->topos) / 2, board->flags);
    }
#endif

    // flip players
    int player = FLAGS_WPLAYER(board->flags) ? BPLAYER : WPLAYER;
    SETPLAYER(player, board->flags);

    // update king position if a king moved
#ifdef CHESSLIB_QWORD_MOVE
    if (MVFROMPC(move) == WKING) {
        SETWKING(MVTOPOS(move), board->flags);
    } else if (MVFROMPC(move) == BKING) {
        SETBKING(MVTOPOS(move), board->flags);
    }
#else
    if (move->frompc == WKING) {
        SETWKING(move->topos, board->flags);
    } else if (move->frompc == BKING) {
        SETBKING(move->topos, board->flags);
    }
#endif

    // update Zobrist signature
    // TODO:
}

int board_is_mate(const board_t *board) {
    pos_t kingpos = FLAGS_WPLAYER(board->flags) ? FLAGS_WKING(board->flags) : FLAGS_BKING(board->flags);

    int incheck = _board_hit(board, kingpos / 8, kingpos % 8, FLAGS_BPLAYER(board->flags));

    if (!incheck) {  // do the easy stuff first
        return 0;
    }

    int ret = 0;
    alst_t *moves = board_get_moves(board);
    ret = moves->len == 0;  // in check and no moves -> mate
#ifdef CHESSLIB_QWORD_MOVE
    alst_free(moves, NULL);
#else
    alst_free(moves, (void (*) (void *)) move_free);
#endif
    return ret;
}

int board_is_stalemate(const board_t *board) {
    pos_t kingpos = FLAGS_WPLAYER(board->flags) ? FLAGS_WKING(board->flags) : FLAGS_BKING(board->flags);

    int incheck = _board_hit(board, kingpos / 8, kingpos % 8, FLAGS_BPLAYER(board->flags));

    // not stalemate if king is in check (easy)
    if (incheck) {
        return 0;
    }

    // check for insufficient material (also easy)
    // 1. get unit counts
    uint32_t rank;
    int rk;
    int offs;
    int pc;
    int counts[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    pos_t wbishop_pos = NOPOS;
    pos_t bbishop_pos = NOPOS;
    for (rk = 7; rk >= 0; --rk) {  // 8 to 1
        rank = board->ranks[rk];
        for (offs = 0; offs < 8; ++offs) {  // a to h
            pc = rank & 0xf;  // one nibble / pc at a time
            if (pc == WBISHOP) {
                wbishop_pos = POS2(offs, rk);
            }
            if (pc == BBISHOP) {
                bbishop_pos = POS2(offs, rk);
            }
            ++counts[pc];
        }
    }

    // 2a. king versus king
    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 0 && counts[WBISHOP] == 0 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 0 && counts[BBISHOP] == 0 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        return 1;
    }

    // 2b. king and bishop versus king
    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 0 && counts[WBISHOP] == 1 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 0 && counts[BBISHOP] == 0 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        return 1;
    }

    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 0 && counts[WBISHOP] == 0 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 0 && counts[BBISHOP] == 1 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        return 1;
    }

    // 2c. king and knight versus king
    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 1 && counts[WBISHOP] == 0 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 0 && counts[BBISHOP] == 0 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        return 1;
    }

    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 0 && counts[WBISHOP] == 0 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 1 && counts[BBISHOP] == 0 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        return 1;
    }

    // 2d. king and bishop versus king and bishop with the bishops on the same color
    if (counts[WPAWN] == 0 && counts[WKNIGHT] == 0 && counts[WBISHOP] == 1 && counts[WROOK] == 0 && counts[WQUEEN] == 0 && counts[WKING] == 1 && \
        counts[BPAWN] == 0 && counts[BKNIGHT] == 0 && counts[BBISHOP] == 1 && counts[BROOK] == 0 && counts[BQUEEN] == 0 && counts[BKING] == 1) {
        if (((wbishop_pos / 8) % 2 == (wbishop_pos % 2)) == ((bbishop_pos / 8) % 2 == (bbishop_pos % 2))) {  // bishops on the same color
            // note that ((pos / 8) % 2 == pos % 2) is True for black squares, False for white squares;
            // i.e., (parity of rk) equals (parity of file) for black squares, d.n.e. for white squares
            return 1;
        }
    }

    // 3. not in check, sufficient mating material; stalemate if no moves (hard)
    int ret = 0;
    alst_t *moves = board_get_moves(board);
    ret = moves->len == 0;  // not in check and no moves -> stalemate
#ifdef CHESSLIB_QWORD_MOVE
    alst_free(moves, NULL);
#else
    alst_free(moves, (void (*) (void *)) move_free);
#endif
    return ret;
}

// returned buffer is static
char *board_to_fen(const board_t *board) {
    // make output buffer
    static char ret[100];
    ret[0] = '\0';
    // encode rank data
    int blanks;
    uint32_t rank;
    char buf[32];
    for (int rk = 8; rk > 0; --rk) {
        // for finding sequences of x's (unoccupied)
        blanks = 0;
        rank = board->ranks[rk - 1];
        for (int offs = 'a'; offs <= 'h'; ++offs) {
            int pc = rank & 0xf;  // encode rank one nibble / file / piece at a time
            if (pc == NOPC) {
                ++blanks;
            } else {
                if (blanks) {
#ifdef __STDC_LIB_EXT1__
                    sprintf_s(buf, sizeof buf, "%d", blanks);
                    strcat_s(ret, sizeof ret, buf);
#else
                    sprintf(buf, "%d", blanks);
                    strcat(ret, buf);
#endif
                    blanks = 0;
                }
#ifdef __STDC_LIB_EXT1__
                strcat_s(ret, sizeof ret, piece_to_str(pc));
#else
                strcat(ret, piece_to_str(pc));
#endif
            }
            rank >>= 4;  // next file
        }
        // flush any trailing blank sequences
        if (blanks) {
#ifdef __STDC_LIB_EXT1__
            sprintf_s(buf, sizeof buf, "%d", blanks);
            strcat_s(ret, sizeof ret, buf);
#else
            sprintf(buf, "%d", blanks);
            strcat(ret, buf);
#endif
        }
        if (rk > 1) {
#ifdef __STDC_LIB_EXT1__
            strcat_s(ret, sizeof ret, "/");
#else
            strcat(ret, "/");
#endif
        }
    }
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, " ");
#else
    strcat(ret, " ");
#endif
    // encode player data
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, FLAGS_WPLAYER(board->flags) ? "w" : "b");
    strcat_s(ret, sizeof ret, " ");
#else
    strcat(ret, FLAGS_WPLAYER(board->flags) ? "w" : "b");
    strcat(ret, " ");
#endif
    // encode castling data
    if (FLAGS_CASTLE(board->flags)) {
#ifdef __STDC_LIB_EXT1__
        if (board->flags & WKCASTLE) strcat_s(ret, sizeof ret, "K");
        if (board->flags & WQCASTLE) strcat_s(ret, sizeof ret, "Q");
        if (board->flags & BKCASTLE) strcat_s(ret, sizeof ret, "k");
        if (board->flags & BQCASTLE) strcat_s(ret, sizeof ret, "q");
#else
        if (board->flags & WKCASTLE) strcat(ret, "K");
        if (board->flags & WQCASTLE) strcat(ret, "Q");
        if (board->flags & BKCASTLE) strcat(ret, "k");
        if (board->flags & BQCASTLE) strcat(ret, "q");
#endif
    } else {
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "-");
#else
        strcat(ret, "-");
#endif
    }
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, " ");
#else
    strcat(ret, " ");
#endif
    // encode en passant data
    char *eppos = pos_to_str(FLAGS_EP(board->flags));
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, eppos);  // NOPOS auto handled
#else
    strcat(ret, eppos);  // NOPOS auto handled
#endif
    // encode turn timer
    // TODO:
    return ret;
}

// returned buffer is static
char *board_to_tui(const board_t *board) {
    static char ret[1024];
    ret[0] = '\0';
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, "    a b c d e f g h\n\n");  // \n is board top/bottom padding
#else
    strcat(ret, "    a b c d e f g h\n\n");  // \n is board top/bottom padding
#endif
    uint32_t rank;
    char buf[10];
    for (signed int rk = 8; rk > 0; --rk) {  // encode each rank from the top down
#ifdef __STDC_LIB_EXT1__
        sprintf_s(buf, sizeof buf, "%d", rk);
        strcat_s(ret, sizeof ret, buf);  // rank label (1-8)
        strcat_s(ret, sizeof ret, "  ");  // board left/right padding
#else
        sprintf(buf, "%d", rk);
        strcat(ret, buf);  // rank label (1-8)
        strcat(ret, "  ");  // board left/right padding
#endif
        rank = board->ranks[rk - 1];
        for (int offs = 'a'; offs <= 'h'; ++offs) {
            // encode the pieces at each file in the rank
#ifdef __STDC_LIB_EXT1__
            strcat_s(ret, sizeof ret, " ");  // file spacing
            strcat_s(ret, sizeof ret, piece_to_str(rank & 0xf));
#else
            strcat(ret, " ");  // file spacing
            strcat(ret, piece_to_str(rank & 0xf));
#endif
            rank >>= 4;  // next file
        }
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "   ");  // board left/right padding
        strcat_s(ret, sizeof ret, buf);  // rank label (1-8)
        strcat_s(ret, sizeof ret, "\n");
#else
        strcat(ret, "   ");  // board left/right padding
        strcat(ret, buf);  // rank label (1-8)
        strcat(ret, "\n");
#endif
    }
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, "\n    a b c d e f g h");  // \n
#else
    strcat(ret, "\n    a b c d e f g h");  // \n
#endif
    return ret;
}
