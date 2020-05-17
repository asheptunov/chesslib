#include <stdio.h>
#include <string.h>

#include "move.h"
#include "algnot.h"
#include "parseutils.h"

move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc) {
    move_t *move = (move_t *) malloc(sizeof(move_t));
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
    if (algnot_parse(algnot, ret)) {
        fprintf(stderr, "move_make_algnot failure to parse string %s", algnot);
        exit(1);
    }
    return ret;
}

void move_free(move_t *move) {
}

int move_cmp(const move_t *a, const move_t *b) {
    if (a->frompc != b->frompc) {
        return a->frompc < b->frompc;
    } else if (a->frompos != b->frompos) {
        return a->frompos < b->frompos;
    } else if (a->topc != b->topc) {
        return a->topc < b->topc;
    } else if (a->topos != b->topos) {
        return a->topos < b->topos;
    } else if (a->killpc != b->killpc) {
        return a->killpc < b->killpc;
    } else if (a->killpos != b->killpos) {
        return a->killpos < b->killpos;
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
    switch(move_is_castle(move)) {
        case WKCASTLE:
        case BKCASTLE:
            return "0-0";
        case WQCASTLE:
        case BQCASTLE:
            return "0-0-0";
    }

    char *ret = (char *) malloc(16 * sizeof(char));
    // strcpy(ret, piece_to_str(move->frompc));
    strcat(ret, pos_to_str(move->frompos));
    if (move_is_cap) strcat(ret, "x");
    // strcat(ret, piece_to_str(move->killpc));
    strcat(ret, pos_to_str(move->topos));
    if (move_is_ep(move)) strcat(ret, "e.p.");
    if (move_is_promo(move)) strcat(ret, piece_to_str(move->topc));
    return ret;
}
