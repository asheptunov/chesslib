#ifndef MOVE_H
#define MOVE_H

#include "defs.h"

typedef struct {
    pos_t frompos;
    pos_t topos;
    pos_t killpos;
    pc_t frompc;
    pc_t topc;
    pc_t killpc;
} move_t;

move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc);

move_t *move_make_algnot(const char *algnot);

move_t *move_cpy(move_t *other);

void move_free(move_t *move);

// int move_equal(const move_t *a, const move_t *b);

int move_cmp(const move_t *a, const move_t *b);

int move_is_cap(const move_t *move);

int move_is_ep(const move_t *move);

int move_is_promo(const move_t *move);

int move_is_castle(const move_t *move);

char *move_str(const move_t *move);

#endif  // MOVE_H
