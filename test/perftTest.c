#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "defs.h"
#include "board.h"

#ifndef INFTY
#define INFTY 100000
#endif

typedef struct {
    move_t *move;
    int utility;
} outcome_t;

static uint64_t ct;

outcome_t *minimax(int (*evaluate) (board_t *), board_t *board, int depth);

outcome_t *minimax(int (*evaluate) (board_t *), board_t *board, int depth) {
    outcome_t *ret = (outcome_t *) malloc(sizeof(outcome_t));
    if (!ret) {
        fprintf(stderr, "malloc error in minimax\n");
        exit(EXIT_FAILURE);
    }
    ret->move = NULL;
    ret->utility = -INFTY;

    if (depth <= 0) {
        ++ct;
        ret->utility = evaluate(board);
        return ret;
    }

    alst_t *moves = board_get_moves(board);

    if (moves->len == 0) {
        alst_free(moves, (void (*) (void *)) move_free);
        ret->utility = -INFTY - depth;
        return ret;
    }

    move_t *move;
    board_t *future_board;
    outcome_t *future_outcome;
    int future_utility;
    for (size_t i = 0; i < moves->len; ++i) {
        move = (move_t *) alst_get(moves, i);
        future_board = board_copy(board);
        board_apply_move(future_board, move);
        future_outcome = minimax(evaluate, future_board, depth - 1);
        if (future_outcome->move) {
            move_free(future_outcome->move);
        }
        board_free(future_board);
        
        future_utility = -(future_outcome->utility);
        if (future_utility > ret->utility) {
            ret->utility = future_utility;
            ret->move = move;
        }

        free(future_outcome);
    }

    if (ret->move) {
        ret->move = move_cpy(ret->move);
    }
    alst_free(moves, (void (*) (void *)) move_free);
    return ret;
}

static const int pcval[] = {1, 3, 3, 5, 9, 0, 1, 3, 3, 5, 9, 0};

int evaluate(board_t *board);

int evaluate(board_t *board) {
    int ret = 0;
    pc_t pc;
    int player = FLAGS_BPLAYER(board->flags);
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            pc = (board->ranks[rank] >> (file * 4)) & 0xf;
            if ((player && pc >= BPAWN) || (!player && pc < BPAWN)) {  // pc is same color as player
                ret += pcval[pc];
            } else {
                ret -= pcval[pc];
            }
        }
    }
    return ret;
}

int main(int argc, char **argv) {
    // setup
    const char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
    board_t *board = board_make(fen);
    const int depth = 3;

    // get outcome
    ct = 0UL;
    clock_t start = clock();
    outcome_t *outcome = minimax(evaluate, board, depth);
    clock_t end = clock();
    double seconds_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;

    // print outcome
    char *tui = board_to_tui(board);
    char *ms = (outcome->move) ? move_str(outcome->move) : "None";
    printf("%s\n", tui);
    printf("fen %s\n", fen);
    printf("searched to depth %d\n", depth);
    printf("best move %s\n", ms);
    printf("utility %d\n", outcome->utility);
    printf("%ld nodes in %f s\n", ct, seconds_elapsed);
    printf("nps ~%.2f\n", ct / seconds_elapsed);

    // cleanup
    if (outcome->move) {
        move_free(outcome->move);
        free(ms);
    }
    board_free(board);
    free(tui);
    free(outcome);
    return EXIT_SUCCESS;
}
