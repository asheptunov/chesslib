#include <stdio.h>
#include <string.h>

#include "board.h"
#include "parseutils.h"

// not in c on wsl; implement using malloc
// implementation from https://stackoverflow.com/questions/46013382/c-strndup-implicit-declaration
char *_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = (char *) malloc(size);
    if (p == NULL) {
        fprintf(stderr, "malloc() error in _strdup\n");
        exit(1);
    }
    memcpy(p, s, size);
    return p;
}

board_t *board_make(const char *fen) {
    board_t *ret = (board_t *) calloc(1, sizeof(board_t));
    char *fencpy = _strdup(fen);  // mutability
    char *board_p = strtok(fencpy, " ");  // split off board data
    char *player_p = strtok(NULL, " ");  // split off player data
    char *castling_p = strtok(NULL, " ");  // split off castling data
    char *ep_p = strtok(NULL, " ");  // split off en passant data
    const char *fenranks[8];
    fenranks[0] = strtok(board_p, "/");  // split board data by ranks
    for (int i = 1; i < 8; ++i) {
        fenranks[i] = strtok(NULL, "/");
    }

    // build each rank from the fen rank data
    int offs, pc;
    for (int rk = 8; rk > 0; --rk) {
        ret->ranks[rk - 1] = 0xcccccccc;  // init to NOPC
        offs = 0;
        for (unsigned int i = 0; i < strlen(fenranks[8 - rk]); ++i) {
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

    free(fencpy);
    return ret;
}

board_t *board_copy(const board_t *other) {
    board_t *ret = (board_t *) malloc(sizeof(board_t));
    if (!ret) {
        fprintf(stderr, "malloc error in board_copy\n");
        exit(1);
    }
    memcpy(&ret->ranks, &other->ranks, sizeof(int32_t) * 8);  // copy ranks
    ret->flags = other->flags;  // copy flags
    return ret;
}

void board_free(const board_t *other) {
    free((void *) other);
}

void board_apply_move(board_t *board, const move_t *move) {
    // kill the target piece if the move is a capture
    if (move_is_cap(move)) {
        int k_rk = move->killpos / 8;
        int k_offs = move->killpos % 8;
        ZEROPOS(k_offs, board->ranks[k_rk]);
        SETPOS(k_offs, board->ranks[k_rk], NOPC);

        // update castling rights / bits if killed piece was an opponent's rook that could've castled
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
    }

    // move the piece
    int f_rk = move->frompos / 8;
    int t_rk = move->topos / 8;
    int f_offs = move->frompos % 8;
    int t_offs = move->topos % 8;
    MOVEPC(f_offs, t_offs, board->ranks[f_rk], board->ranks[t_rk], move->topc);

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

    // disable en passant rights / bits unless made a move that allows en passant
    SETEP(NOPOS, board->flags);
    if ((move->frompc == WPAWN || move->frompc == BPAWN) \
        && (abs(move->topos - move->frompos) == 16)) {
        SETEP((move->frompos + move->topos) / 2, board->flags);
    }

    // flip players
    int player = FLAGS_WPLAYER(board->flags) ? BPLAYER : WPLAYER;
    SETPLAYER(player, board->flags);

    // update king position if a king moved
    if (move->frompc == WKING) {
        SETWKING(move->topos, board->flags);
    } else if (move->frompc == BKING) {
        SETBKING(move->topos, board->flags);
    }

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
    alst_free(moves, (void (*) (void *)) move_free);
    return ret;
}

int board_is_stalemate(const board_t *board) {
    pos_t kingpos = FLAGS_WPLAYER(board->flags) ? FLAGS_WKING(board->flags) : FLAGS_BKING(board->flags);

    int incheck = _board_hit(board, kingpos / 8, kingpos % 8, FLAGS_BPLAYER(board->flags));

    if (incheck) {  // do the easy stuff first
        return 0;
    }

    int ret = 0;
    alst_t *moves = board_get_moves(board);
    ret = moves->len == 0;  // not in check and no moves -> stalemate
    alst_free(moves, (void (*) (void *)) move_free);
    return ret;
}

// caller responsible for freeing returned buffer
char *board_to_fen(const board_t *board) {
    // make output buffer
    char *ret = (char *) malloc(100 * sizeof(char));
    if (!ret) {
        fprintf(stderr, "malloc error in board_to_fen\n");
        exit(1);
    }
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
                    sprintf(buf, "%d", blanks);
                    strcat(ret, buf);
                    blanks = 0;
                }
                strcat(ret, piece_to_str(pc));
            }
            rank >>= 4;  // next file
        }
        // flush any trailing blank sequences
        if (blanks) {
            sprintf(buf, "%d", blanks);
            strcat(ret, buf);
        }
        if (rk > 1) {
            strcat(ret, "/");
        }
    }
    strcat(ret, " ");
    // encode player data
    strcat(ret, FLAGS_WPLAYER(board->flags) ? "w" : "b");
    strcat(ret, " ");
    // encode castling data
    if (FLAGS_CASTLE(board->flags)) {
        if (board->flags & WKCASTLE) strcat(ret, "K");
        if (board->flags & WQCASTLE) strcat(ret, "Q");
        if (board->flags & BKCASTLE) strcat(ret, "k");
        if (board->flags & BQCASTLE) strcat(ret, "q");
    } else {
        strcat(ret, "-");
    }
    strcat(ret, " ");
    // encode en passant data
    char *eppos = pos_to_str(FLAGS_EP(board->flags));
    strcat(ret, eppos);  // NOPOS auto handled
    free(eppos);
    // encode turn timer
    // TODO:
    return ret;
}

// caller responsible for freeing returned buffer
char *board_to_tui(const board_t *board) {
    char *ret = (char *) malloc(1024 * sizeof(char));
    if (!ret) {
        fprintf(stderr, "malloc error in board_to_tui\n");
        exit(1);
    }
    ret[0] = '\0';
    strcat(ret, "    a b c d e f g h\n\n");  // \n is board top/bottom padding
    uint32_t rank;
    char buf[10];
    for (signed int rk = 8; rk > 0; --rk) {  // encode each rank from the top down
        sprintf(buf, "%d", rk);
        strcat(ret, buf);  // rank label (1-8)
        strcat(ret, "  ");  // board left/right padding
        rank = board->ranks[rk - 1];
        for (int offs = 'a'; offs <= 'h'; ++offs) {
            // encode the pieces at each file in the rank
            strcat(ret, " ");  // file spacing
            strcat(ret, piece_to_str(rank & 0xf));
            rank >>= 4;  // next file
        }
        strcat(ret, "   ");  // board left/right padding
        strcat(ret, buf);  // rank label (1-8)
        strcat(ret, "\n");
    }
    strcat(ret, "\n    a b c d e f g h");  // \n
    return ret;
}
