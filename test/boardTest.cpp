extern "C" {
#include "defs.h"
#include "board.h"
#include "move.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::string;

#define FEN_START STARTING_BOARD
#define FEN_E2E4 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3"
#define FEN_C7C5 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6"
#define FEN_POST_W_CASTLE "rn1qk2r/pp2ppbp/3p1np1/1P6/3NP3/2N5/PP3PPP/R1BQ1RK1 b kq -"
#define FEN_POST_B_CASTLE "rn1q1rk1/pp2ppbp/3p1np1/1P6/3NP3/2N5/PP3PPP/R1BQ1RK1 w - -"
#define FEN_RAND_32 "5b2/P1PN1B1r/pnQp4/1p1PPpp1/PqR1p2R/PPr2P1b/pp1k1K1B/4N1n1 w - -"
#define FEN_RAND_8 "7k/Kpp4p/1p4Q1/8/8/B7/8/8 w - -"
#define FEN_EMPTY "8/8/8/8/8/8/8/8 b - -"

#define PRINT_TOP_PAD "    a b c d e f g h\n\n"
#define PRINT_BOT_PAD "    a b c d e f g h"

class BoardTest : public ::testing::Test {
    protected:
        void SetUp() override {
            buildCases =
           {{FEN_START,         {0x31254213, 0x00000000, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0x66666666, 0x978ba879, 1, 1, 1, 1, 1, NOPOS}},
            {FEN_E2E4,          {0x31254213, 0x000c0000, 0xcccccccc, 0xccc0cccc, 0xcccccccc, 0xcccccccc, 0x66666666, 0x978ba879, 1, 1, 1, 1, 0, POS('e', 3)}},
            {FEN_C7C5,          {0x31254213, 0x000c0000, 0xcccccccc, 0xccc0cccc, 0xccccc6cc, 0xcccccccc, 0x66666c66, 0x978ba879, 1, 1, 1, 1, 1, POS('c', 6)}},
            {FEN_POST_W_CASTLE, {0xc53c42c3, 0x000ccc00, 0xccccc1cc, 0xccc01ccc, 0xcccccc0c, 0xc67c6ccc, 0x6866cc66, 0x9ccbac79, 0, 0, 1, 1, 0, NOPOS}},
            {FEN_POST_B_CASTLE, {0xc53c42c3, 0x000ccc00, 0xccccc1cc, 0xccc01ccc, 0xcccccc0c, 0xc67c6ccc, 0x6866cc66, 0xcb9cac79, 0, 0, 0, 0, 1, NOPOS}},
            {FEN_RAND_32,       {0xc7c1cccc, 0x2c5cbc66, 0x8c0cc900, 0x3cc6c3a0, 0xc6600c6c, 0xcccc6476, 0x9c2c10c0, 0xcc8ccccc, 0, 0, 0, 0, 1, NOPOS}},
            {FEN_RAND_8,        {0xcccccccc, 0xcccccccc, 0xccccccc2, 0xcccccccc, 0xcccccccc, 0xc4cccc6c, 0x6cccc665, 0xbccccccc, 0, 0, 0, 0, 1, NOPOS}},
            {FEN_EMPTY,         {0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0, 0, 0, 0, 0, NOPOS}}};
            printCases =
           {{FEN_START,         {"8   r n b q k b n r   8\n", "7   p p p p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - - - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P P P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
            {FEN_E2E4,          {"8   r n b q k b n r   8\n", "7   p p p p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - P - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P - P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
            {FEN_C7C5,          {"8   r n b q k b n r   8\n", "7   p p - p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - p - - - - -   5\n", "4   - - - - P - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P - P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
            {FEN_RAND_32,       {"8   - - - - - b - -   8\n", "7   P - P N - B - r   7\n", "6   p n Q p - - - -   6\n", "5   - p - P P p p -   5\n", "4   P q R - p - - R   4\n", "3   P P r - - P - b   3\n", "2   p p - k - K - B   2\n", "1   - - - - N - n -   1\n\n"}},
            {FEN_EMPTY,         {"8   - - - - - - - -   8\n", "7   - - - - - - - -   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - - - - -   4\n", "3   - - - - - - - -   3\n", "2   - - - - - - - -   2\n", "1   - - - - - - - -   1\n\n"}}};
           applyBasicMoveCases =
            {{{"8/8/8/8/8/2P5/8/8 w KQkq -", "8/8/8/8/2P5/8/8/8 b KQkq -"}, move_make(POS('c', 3), POS('c', 4), NOPOS, WPAWN,   WPAWN,   NOPC)},
            {{"8/8/8/8/8/8/4P3/8 w KQkq -", "8/8/8/8/4P3/8/8/8 b KQkq e3"}, move_make(POS('e', 2), POS('e', 4), NOPOS, WPAWN,   WPAWN,   NOPC)},
            {{"8/8/8/3N4/8/8/8/8 w KQkq -", "8/8/8/8/8/4N3/8/8 b KQkq -"},  move_make(POS('d', 5), POS('e', 3), NOPOS, WKNIGHT, WKNIGHT, NOPC)},
            {{"8/8/8/8/8/8/6B1/8 w KQkq -", "8/1B6/8/8/8/8/8/8 b KQkq -"},  move_make(POS('g', 2), POS('b', 7), NOPOS, WBISHOP, WBISHOP, NOPC)},
            {{"8/8/8/8/8/8/8/7R w KQkq -", "8/8/8/7R/8/8/8/8 b Qkq -"},     move_make(POS('h', 1), POS('h', 5), NOPOS, WROOK,   WROOK,   NOPC)},
            {{"8/8/8/8/8/8/8/R7 w KQkq -", "8/8/8/8/8/8/R7/8 b Kkq -"},     move_make(POS('a', 1), POS('a', 2), NOPOS, WROOK,   WROOK,   NOPC)},
            {{"8/2Q5/8/8/8/8/8/8 w Kkq -", "8/8/8/8/2Q5/8/8/8 b Kkq -"},    move_make(POS('c', 7), POS('c', 4), NOPOS, WQUEEN,  WQUEEN,  NOPC)},
            {{"8/8/8/8/8/8/8/4K3 w KQkq -", "8/8/8/8/8/8/5K2/8 b kq -"},    move_make(POS('e', 1), POS('f', 2), NOPOS, WKING,   WKING,   NOPC)},
            {{"8/3p4/8/8/8/8/8/8 b KQkq -", "8/8/3p4/8/8/8/8/8 w KQkq -"},  move_make(POS('d', 7), POS('d', 6), NOPOS, BPAWN,   BPAWN,   NOPC)},
            {{"8/5p2/8/8/8/8/8/8 b KQkq -", "8/8/8/5p2/8/8/8/8 w KQkq f6"}, move_make(POS('f', 7), POS('f', 5), NOPOS, BPAWN,   BPAWN,   NOPC)},
            {{"8/8/8/8/1n6/8/8/8 b KQkq -", "8/8/8/8/8/3n4/8/8 w KQkq -"},  move_make(POS('b', 4), POS('d', 3), NOPOS, BKNIGHT, BKNIGHT, NOPC)},
            {{"8/3b4/8/8/8/8/8/8 b KQkq b3", "8/8/4b3/8/8/8/8/8 w KQkq -"}, move_make(POS('d', 7), POS('e', 6), NOPOS, BBISHOP, BBISHOP, NOPC)},
            {{"7r/8/8/8/8/8/8/8 b KQkq -", "8/8/8/8/8/7r/8/8 w KQq -"},     move_make(POS('h', 8), POS('h', 3), NOPOS, BROOK,   BROOK,   NOPC)},
            {{"r7/8/8/8/8/8/8/8 b KQkq -", "1r6/8/8/8/8/8/8/8 w KQk -"},    move_make(POS('a', 8), POS('b', 8), NOPOS, BROOK,   BROOK,   NOPC)},
            {{"8/8/8/4q3/8/8/8/8 b Kq c3", "8/8/8/8/8/8/8/q7 w Kq -"},      move_make(POS('e', 5), POS('a', 1), NOPOS, BQUEEN,  BQUEEN,  NOPC)},
            {{"4k3/8/8/8/8/8/8/8 b kq -", "3k4/8/8/8/8/8/8/8 w - -"},       move_make(POS('e', 8), POS('d', 8), NOPOS, BKING,   BKING,   NOPC)}};
           applyBasicCaptureCases =
            {{{"8/8/8/3p4/2P5/8/8/8 w KQkq -", "8/8/8/3P4/8/8/8/8 b KQkq -"}, move_make(POS('c', 4), POS('d', 5), POS('d', 5), WPAWN,   WPAWN,   BPAWN)},
            {{"8/8/8/8/8/2b5/8/1N6 w KQkq f8", "8/8/8/8/8/2N5/8/8 b KQkq -"}, move_make(POS('b', 1), POS('c', 3), POS('c', 3), WKNIGHT, WKNIGHT, BBISHOP)},
            {{"8/8/8/B7/8/2q5/8/8 w KQkq -", "8/8/8/8/8/2B5/8/8 b KQkq -"},   move_make(POS('a', 5), POS('c', 3), POS('c', 3), WBISHOP, WBISHOP, BQUEEN)},
            {{"8/8/8/8/8/8/8/R1n5 w KQkq -", "8/8/8/8/8/8/8/2R5 b Kkq -"},    move_make(POS('a', 1), POS('c', 1), POS('c', 1), WROOK,   WROOK,   BKNIGHT)},
            {{"8/8/8/2Q2r2/8/8/8/8 w k -", "8/8/8/5Q2/8/8/8/8 b k -"},        move_make(POS('c', 5), POS('f', 5), POS('f', 5), WQUEEN,  WQUEEN,  BROOK)},
            {{"8/8/8/8/8/8/3n4/4K3 w KQ c6", "8/8/8/8/8/8/3K4/8 b - -"},      move_make(POS('e', 1), POS('d', 2), POS('d', 2), WKING,   WKING,   BKNIGHT)},
            {{"8/8/8/8/8/8/3p4/2R5 b - g3", "8/8/8/8/8/8/8/2q5 w - -"},       move_make(POS('d', 2), POS('c', 1), POS('c', 1), BPAWN,   BQUEEN,  WROOK)},
            {{"8/8/3n4/8/4B3/8/8/8 b Qkq -", "8/8/8/8/4n3/8/8/8 w Qkq -"},    move_make(POS('d', 6), POS('e', 4), POS('e', 4), BKNIGHT, BKNIGHT, WBISHOP)},
            {{"8/b7/8/8/8/8/5P2/8 b Kkq -", "8/8/8/8/8/8/5b2/8 w Kkq -"},     move_make(POS('a', 7), POS('f', 2), POS('f', 2), BBISHOP, BBISHOP, WPAWN)},
            {{"r1Q5/8/8/8/8/8/8/8 b Kq -", "2r5/8/8/8/8/8/8/8 w K -"},        move_make(POS('a', 8), POS('c', 8), POS('c', 8), BROOK,   BROOK,   WQUEEN)},
            {{"8/8/8/8/8/8/5q2/5R2 b - b3", "8/8/8/8/8/8/8/5q2 w - -"},       move_make(POS('f', 2), POS('f', 1), POS('f', 1), BQUEEN,  BQUEEN,  WROOK)},
            {{"4k3/3N4/8/8/8/8/8/8 b Qkq -", "8/3k4/8/8/8/8/8/8 w Q -"},      move_make(POS('e', 8), POS('d', 7), POS('d', 7), BKING,   BKING,   WKNIGHT)},
            {{"rnbq1k1r/pp1Pbppp/2p5/8/1PB5/8/P1P1NnPP/RNBQK2R b KQ -", "rnbq1k1r/pp1Pbppp/2p5/8/1PB5/8/P1P1N1PP/RNBQK2n w Q -"}, move_make(POS('f', 2), POS('h', 1), POS('h', 1), BKNIGHT, BKNIGHT, WROOK)}};  // if an uncastled rook is captured, can't castle on that side anymore
           applyCastlingCases =
            {{{"8/8/8/8/8/8/8/4K2R w K -", "8/8/8/8/8/8/8/5RK1 b - -"},   move_make(POS('e', 1), POS('g', 1), NOPOS, WKING, WKING, NOPC)},
            {{"8/8/8/8/8/8/8/R3K3 w Qkq -", "8/8/8/8/8/8/8/2KR4 b kq -"}, move_make(POS('e', 1), POS('c', 1), NOPOS, WKING, WKING, NOPC)},
            {{"4k2r/8/8/8/8/8/8/8 b KQk -", "5rk1/8/8/8/8/8/8/8 w KQ -"}, move_make(POS('e', 8), POS('g', 8), NOPOS, BKING, BKING, NOPC)},
            {{"r3k3/8/8/8/8/8/8/8 b q -", "2kr4/8/8/8/8/8/8/8 w - -"},    move_make(POS('e', 8), POS('c', 8), NOPOS, BKING, BKING, NOPC)}};
           applyEnPassantCases =
            {{{"8/8/8/5Pp1/8/8/8/8 w KQkq g6", "8/8/6P1/8/8/8/8/8 b KQkq -"}, move_make(POS('f', 5), POS('g', 6), POS('g', 5), WPAWN, WPAWN, BPAWN)},
            {{"8/8/8/8/Pp6/8/8/8 b KQkq a3", "8/8/8/8/8/p7/8/8 w KQkq -"},    move_make(POS('b', 4), POS('a', 3), POS('a', 4), BPAWN, BPAWN, WPAWN)}};
           applyPromotionCases =
            {{{"8/3P4/8/8/8/8/8/8 w KQkq a6", "3Q4/8/8/8/8/8/8/8 b KQkq -"}, move_make(POS('d', 7), POS('d', 8), NOPOS, WPAWN, WQUEEN,  NOPC)},
            {{"8/1P6/8/8/8/8/8/8 w KQk d6", "1R6/8/8/8/8/8/8/8 b KQk -"},    move_make(POS('b', 7), POS('b', 8), NOPOS, WPAWN, WROOK,   NOPC)},
            {{"8/7P/8/8/8/8/8/8 w KQ g6", "7B/8/8/8/8/8/8/8 b KQ -"},        move_make(POS('h', 7), POS('h', 8), NOPOS, WPAWN, WBISHOP, NOPC)},
            {{"8/5P2/8/8/8/8/8/8 w K -", "5N2/8/8/8/8/8/8/8 b K -"},         move_make(POS('f', 7), POS('f', 8), NOPOS, WPAWN, WKNIGHT, NOPC)},
            {{"8/8/8/8/8/8/1p6/8 b Qkq b3", "8/8/8/8/8/8/8/1q6 w Qkq -"},    move_make(POS('b', 2), POS('b', 1), NOPOS, BPAWN, BQUEEN,  NOPC)},
            {{"8/8/8/8/8/8/3p4/8 b kq e3", "8/8/8/8/8/8/8/3r4 w kq -"},      move_make(POS('d', 2), POS('d', 1), NOPOS, BPAWN, BROOK,   NOPC)},
            {{"8/8/8/8/8/8/5p2/8 b q h3", "8/8/8/8/8/8/8/5b2 w q -"},        move_make(POS('f', 2), POS('f', 1), NOPOS, BPAWN, BBISHOP, NOPC)},
            {{"8/8/8/8/8/8/p7/8 b - -", "8/8/8/8/8/8/8/n7 w - -"},           move_make(POS('a', 2), POS('a', 1), NOPOS, BPAWN, BKNIGHT, NOPC)}};
        }

#ifndef CHESSLIB_QWORD_MOVE
        void TearDown() override {
            for (auto it = applyBasicMoveCases.begin(); it != applyBasicMoveCases.end(); ++it) {
                move_free(it->second);
            }
            for (auto it = applyBasicCaptureCases.begin(); it != applyBasicCaptureCases.end(); ++it) {
                move_free(it->second);
            }
            for (auto it = applyCastlingCases.begin(); it != applyCastlingCases.end(); ++it) {
                move_free(it->second);
            }
            for (auto it = applyEnPassantCases.begin(); it != applyEnPassantCases.end(); ++it) {
                move_free(it->second);
            }
            for (auto it = applyPromotionCases.begin(); it != applyPromotionCases.end(); ++it) {
                move_free(it->second);
            }
        }
#endif

    map<string, vector<unsigned int>> buildCases;
    map<string, vector<string>> printCases;
#ifdef CHESSLIB_QWORD_MOVE
    map<vector<string>, move_t> applyBasicMoveCases;
    map<vector<string>, move_t> applyBasicCaptureCases;
    map<vector<string>, move_t> applyCastlingCases;
    map<vector<string>, move_t> applyEnPassantCases;
    map<vector<string>, move_t> applyPromotionCases;
#else
    map<vector<string>, move_t *> applyBasicMoveCases;
    map<vector<string>, move_t *> applyBasicCaptureCases;
    map<vector<string>, move_t *> applyCastlingCases;
    map<vector<string>, move_t *> applyEnPassantCases;
    map<vector<string>, move_t *> applyPromotionCases;
#endif
};

TEST_F(BoardTest, ValConstruct) {
    board_t *b;
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        b = board_make(it->first.c_str());
        for (int i = 0; i < 8; ++i) {
            EXPECT_EQ(b->ranks[i], it->second[i]) << "diff at rank " << i << " on build" << endl;
        }
        EXPECT_EQ(FLAGS_WKCASTLE(b->flags), it->second[8]) << "wrong white kingside castling privilege bit" << endl;
        EXPECT_EQ(FLAGS_WQCASTLE(b->flags), it->second[9]) << "wrong white queenside castling privilege bit" << endl;
        EXPECT_EQ(FLAGS_BKCASTLE(b->flags), it->second[10]) << "wrong black kingside castling privilege bit" << endl;
        EXPECT_EQ(FLAGS_BQCASTLE(b->flags), it->second[11]) << "wrong black queenside castling privilege bit" << endl;
        EXPECT_EQ(FLAGS_WPLAYER(b->flags), it->second[12]) << "wrong player bit" << endl;
        EXPECT_NE(FLAGS_BPLAYER(b->flags), it->second[12]) << "both white and black player bits set" << endl;
        EXPECT_EQ(FLAGS_EP(b->flags), it->second[13]) << "wrong ep position" << endl;

        // cleanup
        board_free(b);
    }
}

TEST_F(BoardTest, CpyConstruct) {
    board_t *b1;
    board_t *b2;
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        b1 = board_make(it->first.c_str());
        b2 = board_copy(b1);
        for (int i = 0; i < 8; ++i) {
            EXPECT_EQ(b2->ranks[i], b1->ranks[i]) << "diff at rank " << i << " on cpy construct" << endl;
        }
        // EXPECT_EQ(b2->flags, b1->flags) << "diff flags on cpy construct" << endl;
        EXPECT_TRUE(b2->flags == b1->flags);

        // cleanup
        board_free(b1);
        board_free(b2);
    }
}

TEST_F(BoardTest, MakeFen) {
    board_t *b;
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        b = board_make(it->first.c_str());
        char *fen = board_to_fen(b);
        EXPECT_EQ(it->first, fen) << "unexpected fen " << fen << " generated by board which was constructed using fen " << it->first << endl;

        // cleanup
        board_free(b);
    }
}

TEST_F(BoardTest, PrintOp) {
    board_t *b;
    for (auto it = printCases.begin(); it != printCases.end(); ++it) {
        // expected
        string expect(PRINT_TOP_PAD);
        for (int i = 0; i < 8; ++i) {
            expect.append(it->second[i]);
        }
        expect.append(PRINT_BOT_PAD);

        // actual
        b = board_make(it->first.c_str());
        char *tui = board_to_tui(b);
        EXPECT_EQ(tui, expect) << "unexpected tui " << tui << " generated by board which was constructed using fen " << it->first << "; expected tui " << expect << endl;

        // cleanup
        board_free(b);
    }
}

#ifndef APPLY_AND_TEST_CASES
#define APPLY_AND_TEST_CASES(cases) \
    for (auto it = cases.begin(); it != cases.end(); ++it) { \
        /* construct a board from the starting fen */ \
        board_t *b = board_make(it->first[0].c_str()); \
        /* apply the move */ \
        board_apply_move(b, it->second); \
        /* fen after applying the move should match expected */ \
        char *fen = board_to_fen(b); \
        EXPECT_EQ(fen, it->first[1]) << "unexpected fen " << fen << " after applying move " << it->second << " to board with fen " << it->first[0] << endl; \
        \
        /* cleanup */ \
        board_free(b); \
    }
#endif

TEST_F(BoardTest, ApplyBasic) {
    APPLY_AND_TEST_CASES(applyBasicMoveCases);
}

TEST_F(BoardTest, ApplyCaptureBasic) {
    APPLY_AND_TEST_CASES(applyBasicCaptureCases);
}

TEST_F(BoardTest, ApplyCastling) {
    APPLY_AND_TEST_CASES(applyCastlingCases);
}

TEST_F(BoardTest, ApplyEnPassant) {
    APPLY_AND_TEST_CASES(applyEnPassantCases);
}

TEST_F(BoardTest, ApplyPromotion) {
    APPLY_AND_TEST_CASES(applyPromotionCases);
}
