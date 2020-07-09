#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "algnot.h"
#include "parseutils.h"

#ifdef CHESSLIB_QWORD_MOVE
move_t move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc) {
    return MVMAKE(frompos, topos, killpos, frompc, topc, killpc);
}
#else
move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc) {
    move_t *move = (move_t *) malloc(sizeof(move_t));
    if (!move) {
        fprintf(stderr, "malloc error in move_make\n");
        exit(EXIT_FAILURE);
    }
    move->frompos = frompos;
    move->topos = topos;
    move->killpos = killpos;
    move->frompc = frompc;
    move->topc = topc;
    move->killpc = killpc;
    return move;
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
move_t move_make_algnot(const char *algnot) {
    move_t ret = 0;
    if (algnot_parse(algnot, &ret)) {
        fprintf(stderr, "move_make_algnot failure to parse string %s", algnot);
        exit(EXIT_FAILURE);
    }
    return ret;
}
#else
move_t *move_make_algnot(const char *algnot) {
    move_t *ret = (move_t *) malloc(sizeof(move_t));
    if (!ret) {
        fprintf(stderr, "malloc error in move_make_algnot\n");
        exit(EXIT_FAILURE);
    }
    if (algnot_parse(algnot, ret)) {
        fprintf(stderr, "move_make_algnot failure to parse string %s", algnot);
        exit(EXIT_FAILURE);
    }
    return ret;
}
#endif

#ifndef CHESSLIB_QWORD_MOVE
move_t *move_cpy(move_t *other) {
    move_t *cpy = (move_t *) malloc(sizeof(move_t));
    if (!cpy) {
        fprintf(stderr, "malloc error in move_cpy\n");
        exit(EXIT_FAILURE);
    }
    memcpy(cpy, other, sizeof(move_t));
    return cpy;
}
#endif

#ifndef CHESSLIB_QWORD_MOVE
void move_free(move_t *move) {
    free(move);
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
int move_cmp(const move_t a, const move_t b) {
    if        (MVFROMPC(a)  != MVFROMPC(b)) {
        return MVFROMPC(a)  -  MVFROMPC(b);
    } else if (MVFROMPOS(a) != MVFROMPOS(b)) {
        return MVFROMPOS(a) -  MVFROMPOS(b);
    } else if (MVTOPC(a)    != MVTOPC(b)) {
        return MVTOPC(a)    -  MVTOPC(b);
    } else if (MVTOPOS(a)   != MVTOPOS(b)) {
        return MVTOPOS(a)   -  MVTOPOS(b);
    } else if (MVKILLPC(a)  != MVKILLPC(b)) {
        return MVKILLPC(a)  -  MVKILLPC(b);
    } else if (MVKILLPOS(a) != MVKILLPOS(b)) {
        return MVKILLPOS(a) -  MVKILLPOS(b);
    } else {  // equal
        return 0;
    }
}
#else
int move_cmp(const move_t *a, const move_t *b) {
    if (a->frompc != b->frompc) {
        return a->frompc - b->frompc;
    } else if (a->frompos != b->frompos) {
        return a->frompos - b->frompos;
    } else if (a->topc != b->topc) {
        return a->topc - b->topc;
    } else if (a->topos != b->topos) {
        return a->topos - b->topos;
    } else if (a->killpc != b->killpc) {
        return a->killpc - b->killpc;
    } else if (a->killpos != b->killpos) {
        return a->killpos - b->killpos;
    } else {  // equal
        return 0;
    }
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
int move_is_cap(const move_t move) {
    return MVKILLPC(move) != NOPC;
}
#else
int move_is_cap(const move_t *move) {
    return move->killpc != NOPC;
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
int move_is_ep(const move_t move) {
    return move_is_cap(move) && (MVTOPOS(move) != MVKILLPOS(move));
}
#else
int move_is_ep(const move_t *move) {
    return move_is_cap(move) && (move->topos != move->killpos);
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
int move_is_promo(const move_t move) {
    return MVFROMPC(move) != MVTOPC(move);
}
#else
int move_is_promo(const move_t *move) {
    return move->frompc != move->topc;
}
#endif

#ifdef CHESSLIB_QWORD_MOVE
int move_is_castle(const move_t move) {
    switch(move & MVMAKE(0xff, 0xff, 0x00, 0xff, 0x00, 0x00)) {  // only care about frompos, topos, and frompc
        case MVMAKE(POS('e', 1), POS('g', 1), 0, WKING, 0, 0): return WKCASTLE;
        case MVMAKE(POS('e', 1), POS('c', 1), 0, WKING, 0, 0): return WQCASTLE;
        case MVMAKE(POS('e', 8), POS('g', 8), 0, BKING, 0, 0): return BKCASTLE;
        case MVMAKE(POS('e', 8), POS('c', 8), 0, BKING, 0, 0): return BQCASTLE;
        default: return 0;
    }
}
#else
int move_is_castle(const move_t *move) {
    if (move->frompc == WKING) {
        if (move->frompos == POS('e', 1) && move->topos == POS('g', 1)) {
            return WKCASTLE;
        }
        if (move->frompos == POS('e', 1) && move->topos == POS('c', 1)) {
            return WQCASTLE;
        }
    }
    if (move->frompc == BKING) {
        if (move->frompos == POS('e', 8) && move->topos == POS('g', 8)) {
            return BKCASTLE;
        }
        if (move->frompos == POS('e', 8) && move->topos == POS('c', 8)) {
            return BQCASTLE;
        }
    }
    return 0;  // not castling move
}
#endif

// lossy algebraic
#ifdef CHESSLIB_QWORD_MOVE
char *move_str(const move_t move) {
#else
char *move_str(const move_t *move) {
#endif
    static char ret[16];
    switch(move_is_castle(move)) {
        case WKCASTLE:
        case BKCASTLE:
#ifdef __STDC_LIB_EXT1__
            strcpy_s(ret, sizeof ret, "0-0");
#else
            strcpy(ret, "0-0");
#endif
            return ret;
        case WQCASTLE:
        case BQCASTLE:
#ifdef __STDC_LIB_EXT1__
            strcpy_s(ret, sizeof ret, "0-0-0");
#else
            strcpy(ret, "0-0-0");
#endif
            return ret;
    }
    char *pos_str;
    ret[0] = '\0';
#ifdef CHESSLIB_QWORD_MOVE
    pos_str = pos_to_str(MVFROMPOS(move));
#else
    pos_str = pos_to_str(move->frompos);
#endif
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, pos_str);
#else
    strcat(ret, pos_str);
#endif
    if (move_is_cap(move)) {
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "x");
#else
        strcat(ret, "x");
#endif
    }
#ifdef CHESSLIB_QWORD_MOVE
    pos_str = pos_to_str(MVTOPOS(move));
#else
    pos_str = pos_to_str(move->topos);
#endif
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, pos_str);
#else
    strcat(ret, pos_str);
#endif
    if (move_is_ep(move)) {
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "e.p.");
#else
        strcat(ret, "e.p.");
#endif
    }
    if (move_is_promo(move)) {
#ifdef __STDC_LIB_EXT1__
#ifdef CHESSLIB_QWORD_MOVE
        strcat_s(ret, sizeof ret, piece_to_str(MVTOPC(move)));
#else
        strcat_s(ret, sizeof ret, piece_to_str(move->topc));
#endif
#else
#ifdef CHESSLIB_QWORD_MOVE
        strcat(ret, piece_to_str(MVTOPC(move)));
#else
        strcat(ret, piece_to_str(move->topc));
#endif
#endif
    }
    return ret;
}

// lossless algebraic (recoverable)
#ifdef CHESSLIB_QWORD_MOVE
char *move_algnot(const move_t move) {
#else
char *move_algnot(const move_t *move) {
#endif
    static char ret[16];
    char *pos_str;
    ret[0] = '\0';
#ifdef __STDC_LIB_EXT1__
#ifdef CHESSLIB_QWORD_MOVE
    strcat_s(ret, sizeof ret, piece_to_str(MVFROMPC(move)));
#else
    strcat_s(ret, sizeof ret, piece_to_str(move->frompc));
#endif
#else
#ifdef CHESSLIB_QWORD_MOVE
    strcat(ret, piece_to_str(MVFROMPC(move)));
#else
    strcat(ret, piece_to_str(move->frompc));
#endif
#endif
#ifdef CHESSLIB_QWORD_MOVE
    pos_str = pos_to_str(MVFROMPOS(move));
#else
    pos_str = pos_to_str(move->frompos);
#endif
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, pos_str);
#else
    strcat(ret, pos_str);
#endif
    if (move_is_cap(move)) {
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "x");
#else
        strcat(ret, "x");
#endif
    }
#ifdef __STDC_LIB_EXT1__
#ifdef CHESSLIB_QWORD_MOVE
    if (MVKILLPC(move) != NOPC) {
        strcat_s(ret, sizeof ret, piece_to_str(MVKILLPC(move)));
    }
#else
    if (move->killpc != NOPC) {
        strcat_s(ret, sizeof ret, piece_to_str(move->killpc));
    }
#endif
#else
#ifdef CHESSLIB_QWORD_MOVE
    if (MVKILLPC(move) != NOPC) {
        strcat(ret, piece_to_str(MVKILLPC(move)));
    }
#else
    if (move->killpc != NOPC) {
        strcat(ret, piece_to_str(move->killpc));
    }
#endif
#endif
#ifdef CHESSLIB_QWORD_MOVE
    pos_str = pos_to_str(MVTOPOS(move));
#else
    pos_str = pos_to_str(move->topos);
#endif
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, pos_str);
#else
    strcat(ret, pos_str);
#endif
    if (move_is_ep(move)) {
#ifdef __STDC_LIB_EXT1__
        strcat_s(ret, sizeof ret, "e.p.");
#else
        strcat(ret, "e.p.");
#endif
    }
    if (move_is_promo(move)) {
#ifdef __STDC_LIB_EXT1__
#ifdef CHESSLIB_QWORD_MOVE
        strcat_s(ret, sizeof ret, piece_to_str(MVTOPC(move)));
#else
        strcat_s(ret, sizeof ret, piece_to_str(move->topc));
#endif
#else
#ifdef CHESSLIB_QWORD_MOVE
        strcat(ret, piece_to_str(MVTOPC(move)));
#else
        strcat(ret, piece_to_str(move->topc));
#endif
#endif
    }
    return ret;
}
