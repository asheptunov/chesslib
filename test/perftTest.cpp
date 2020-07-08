extern "C" {
#include "defs.h"
#include "board.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <ctime>
#include <vector>

#define CHESS_INFTY 100000

uint64_t search(board_t *board, int depth) {
    if (depth <= 0) {
        // pseudo-leaf (hit depth limit)
        // this is counted by perft
        return 1;
    }
    alst_t *moves = board_get_moves(board);
    if (moves->len == 0) {
        // this isn't counted by perft
#ifdef CHESSLIB_QWORD_MOVE
        alst_free(moves, NULL);
#else
        alst_free(moves, (void (*) (void *)) move_free);
#endif
        return 0;
    }
    uint64_t ct = 0;
    for (size_t i = 0; i < moves->len; ++i) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t move = (move_t) alst_get(moves, i);
#else
        move_t *move = (move_t *) alst_get(moves, i);
#endif
        board_t *future_board = board_copy(board);
        board_apply_move(future_board, move);
        ct += search(future_board, depth - 1);
        board_free(future_board);
    }
#ifdef CHESSLIB_QWORD_MOVE
    alst_free(moves, NULL);
#else
    alst_free(moves, (void (*) (void *)) move_free);
#endif
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

static const uint64_t THRESH = 100000000;  // 100M
#define verify_perft_n(fen) \
    board_t *board = board_make(fen); \
    for (size_t i = 0; i < (sizeof(expected_counts) / sizeof(expected_counts[0])); ++i) { \
        if (expected_counts[i] <= THRESH) { \
            EXPECT_EQ(search(board, i), expected_counts[i]); \
        } \
    } \
    board_free(board);

/**
* Counts given at https://www.chessprogramming.org/Perft_Results
*/

TEST(PerftTest, CorrectnessPerft1) {
    const uint64_t expected_counts[] = {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860};
    verify_perft_n(STARTING_BOARD);
}

TEST(PerftTest, CorrectnessPerft2) {
    const uint64_t expected_counts[] = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685};
    verify_perft_n("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");
}

TEST(PerftTest, CorrectnessPerft3) {
    const uint64_t expected_counts[] = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393};
    verify_perft_n("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -");
}

TEST(PerftTest, CorrectnessPerft4) {
    const uint64_t expected_counts[] = {1, 6, 264, 9467, 422333, 15833292, 706045033};
    verify_perft_n("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -");
}

TEST(PerftTest, CorrectnessPerft5) {
    const uint64_t expected_counts[] = {1, 44, 1486, 62379, 2103487, 89941194};
    verify_perft_n("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -")
}

TEST(PerftTest, CorrectnessPerft6) {
    const uint64_t expected_counts[] = {1, 46, 2079, 89890, 3894594, 164075551, 6923051137};
    verify_perft_n("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -");
}

TEST(PerftTest, SpeedPerft2) {
    const char *fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    board_t *board = board_make(fen);
    float nps_actual = nps(board, 4, 3);
    float nps_expect = 100000;
    board_free(board);
    std::cerr << "[          ] mean c++ nps " << nps_actual << std::endl;
    EXPECT_GT(nps_actual, nps_expect) << "nps too low, expected at least " << nps_expect << " but got " << nps_actual << std::endl;
}
