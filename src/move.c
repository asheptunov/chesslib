#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "algnot.h"
#include "parseutils.h"

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
    // return (move_t) {frompos, topos, killpos, frompc, topc, killpc};
}

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

move_t *move_cpy(move_t *other) {
    move_t *cpy = (move_t *) malloc(sizeof(move_t));
    if (!cpy) {
        fprintf(stderr, "malloc error in move_cpy\n");
        exit(EXIT_FAILURE);
    }
    memcpy(cpy, other, sizeof(move_t));
    return cpy;
}

void move_free(move_t *move) {
    free(move);
}

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

int move_is_cap(const move_t *move) {
    return move->killpc != NOPC;
}

int move_is_ep(const move_t *move) {
    return move_is_cap(move) && (move->topos != move->killpos);
}

int move_is_promo(const move_t *move) {
    return move->frompc != move->topc;
}

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

char *move_str(const move_t *move) {
    static char ret[16];
#ifndef CHESSLIB_PRODUCTION
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
# else  // ifdef CHESSLIB_PRODUCTION
# endif  // ifndef CHESSLIB_PRODUCTION
    char *pos_str;
    ret[0] = '\0';
#ifdef CHESSLIB_PRODUCTION
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, piece_to_str(move->frompc));
#else
    strcat(ret, piece_to_str(move->frompc));
#endif
#else  // ifndef CHESSLIB_PRODUCTION
#endif  // ifdef CHESSLIB_PRODUCTION
    pos_str = pos_to_str(move->frompos);
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
#ifdef CHESSLIB_PRODUCTION
#ifdef __STDC_LIB_EXT1__
    strcat_s(ret, sizeof ret, piece_to_str(move->killpc));
#else
    strcat(ret, piece_to_str(move->killpc));
#endif
#else  // ifndef CHESSLIB_PRODUCTION
#endif  // ifdef CHESSLIB_PRODUCTION
    pos_str = pos_to_str(move->topos);
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
        strcat_s(ret, sizeof ret, piece_to_str(move->topc));
#else
        strcat(ret, piece_to_str(move->topc));
#endif
    }
    return ret;
}
