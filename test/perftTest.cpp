extern "C" {
#include "defs.h"
#include "board.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <vector>

#ifndef CHESS_INFTY
#define CHESS_INFTY 100000
#endif

uint64_t search(board_t *board, int depth) {
    if (depth <= 0) {
        // pseudo-leaf (hit depth limit)
        // this is counted by perft
        return 1;
    }
    alst_t *moves = board_get_moves(board);
    if (moves->len == 0) {
        // this isn't counted by perft
        alst_free(moves, (void (*) (void *)) move_free);
        return 0;
    }
    uint64_t ct = 0;
    for (size_t i = 0; i < moves->len; ++i) {
        move_t *move = (move_t *) alst_get(moves, i);
        board_t *future_board = board_copy(board);
        board_apply_move(future_board, move);
        ct += search(future_board, depth - 1);
        board_free(future_board);
    }
    alst_free(moves, (void (*) (void *)) move_free);
    return ct;
}

float nps(board_t *board, int depth, int samples) {
    uint64_t ndsum = 0L;
    double secsum = 0.0;
    for (int i = 0; i < samples; ++i) {
        clock_t start = clock();
        ndsum += search(board, depth);
        clock_t end = clock();
        secsum += ((double) (end - start)) / CLOCKS_PER_SEC;
    }
    return (float) (ndsum / secsum);
}

TEST(PerftTest, SpeedPerft2) {
    const char *fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    board_t *board = board_make(fen);
    float nps_actual = nps(board, 4, 3);
    float nps_expect = 100000;
    board_free(board);
    EXPECT_GT(nps_actual, nps_expect) << "nps too low, expected at least " << nps_expect << " but got " << nps_actual << std::endl;
}
