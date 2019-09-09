#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>

#include "board.h"
#include "defs.h"
#include "move.h"

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::string;
using game::Move;
using game::Board;

#define FEN_START STARTING_BOARD
#define FEN_E2E4 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3"
#define FEN_C7C5 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6"
#define FEN_POST_W_CASTLE "rn1qk2r/pp2ppbp/3p1np1/1P6/3NP3/2N5/PP3PPP/R1BQ1RK1 b kq -"
#define FEN_POST_B_CASTLE "rn1q1rk1/pp2ppbp/3p1np1/1P6/3NP3/2N5/PP3PPP/R1BQ1RK1 w - -"
#define FEN_RAND_32 "5b2/P1PN1B1r/pnQp4/1p1PPpp1/PqR1p2R/PPr2P1b/pp1k1K1B/4N1n1 w - -"
#define FEN_RAND_8 "7k/Kpp4p/1p4Q1/8/8/B7/8/8 w - -"
#define FEN_EMPTY "8/8/8/8/8/8/8/8 b - -"

static map<string, vector<unsigned int>> buildCases =
   {{FEN_START,         {0x31254213, 0x00000000, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0x66666666, 0x978ba879, 1, 1, 1, 1, 1, NOPOS}},
    {FEN_E2E4,          {0x31254213, 0x000c0000, 0xcccccccc, 0xccc0cccc, 0xcccccccc, 0xcccccccc, 0x66666666, 0x978ba879, 1, 1, 1, 1, 0, POS('e', 3)}},
    {FEN_C7C5,          {0x31254213, 0x000c0000, 0xcccccccc, 0xccc0cccc, 0xccccc6cc, 0xcccccccc, 0x66666c66, 0x978ba879, 1, 1, 1, 1, 1, POS('c', 6)}},
    {FEN_POST_W_CASTLE, {0xc53c42c3, 0x000ccc00, 0xccccc1cc, 0xccc01ccc, 0xcccccc0c, 0xc67c6ccc, 0x6866cc66, 0x9ccbac79, 0, 0, 1, 1, 0, NOPOS}},
    {FEN_POST_B_CASTLE, {0xc53c42c3, 0x000ccc00, 0xccccc1cc, 0xccc01ccc, 0xcccccc0c, 0xc67c6ccc, 0x6866cc66, 0xcb9cac79, 0, 0, 0, 0, 1, NOPOS}},
    {FEN_RAND_32,       {0xc7c1cccc, 0x2c5cbc66, 0x8c0cc900, 0x3cc6c3a0, 0xc6600c6c, 0xcccc6476, 0x9c2c10c0, 0xcc8ccccc, 0, 0, 0, 0, 1, NOPOS}},
    {FEN_RAND_8,        {0xcccccccc, 0xcccccccc, 0xccccccc2, 0xcccccccc, 0xcccccccc, 0xc4cccc6c, 0x6cccc665, 0xbccccccc, 0, 0, 0, 0, 1, NOPOS}},
    {FEN_EMPTY,         {0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0xcccccccc, 0, 0, 0, 0, 0, NOPOS}}};

#define PRINT_TOP_PAD "    a b c d e f g h\n\n"
#define PRINT_BOT_PAD "    a b c d e f g h"

static map<string, vector<string>> printCases =
   {{FEN_START,         {"8   r n b q k b n r   8\n", "7   p p p p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - - - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P P P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
    {FEN_E2E4,          {"8   r n b q k b n r   8\n", "7   p p p p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - P - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P - P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
    {FEN_C7C5,          {"8   r n b q k b n r   8\n", "7   p p - p p p p p   7\n", "6   - - - - - - - -   6\n", "5   - - p - - - - -   5\n", "4   - - - - P - - -   4\n", "3   - - - - - - - -   3\n", "2   P P P P - P P P   2\n", "1   R N B Q K B N R   1\n\n"}},
    {FEN_RAND_32,       {"8   - - - - - b - -   8\n", "7   P - P N - B - r   7\n", "6   p n Q p - - - -   6\n", "5   - p - P P p p -   5\n", "4   P q R - p - - R   4\n", "3   P P r - - P - b   3\n", "2   p p - k - K - B   2\n", "1   - - - - N - n -   1\n\n"}},
    {FEN_EMPTY,         {"8   - - - - - - - -   8\n", "7   - - - - - - - -   7\n", "6   - - - - - - - -   6\n", "5   - - - - - - - -   5\n", "4   - - - - - - - -   4\n", "3   - - - - - - - -   3\n", "2   - - - - - - - -   2\n", "1   - - - - - - - -   1\n\n"}}};

static map<vector<string>, Move> applyBasicMoveCases =
   {{{"8/8/8/8/8/2P5/8/8 w KQkq -", "8/8/8/8/2P5/8/8/8 b KQkq -"}, Move(POS('c', 3), POS('c', 4), NOPOS, WPAWN, WPAWN, NOPC)},
    {{"8/8/8/8/8/8/4P3/8 w KQkq -", "8/8/8/8/4P3/8/8/8 b KQkq e3"}, Move(POS('e', 2), POS('e', 4), NOPOS, WPAWN, WPAWN, NOPC)},
    {{"8/8/8/3N4/8/8/8/8 w KQkq -", "8/8/8/8/8/4N3/8/8 b KQkq -"}, Move(POS('d', 5), POS('e', 3), NOPOS, WKNIGHT, WKNIGHT, NOPC)},
    {{"8/8/8/8/8/8/6B1/8 w KQkq -", "8/1B6/8/8/8/8/8/8 b KQkq -"}, Move(POS('g', 2), POS('b', 7), NOPOS, WBISHOP, WBISHOP, NOPC)},
    {{"8/8/8/8/8/8/8/7R w KQkq -", "8/8/8/7R/8/8/8/8 b Qkq -"}, Move(POS('h', 1), POS('h', 5), NOPOS, WROOK, WROOK, NOPC)},
    {{"8/8/8/8/8/8/8/R7 w KQkq -", "8/8/8/8/8/8/R7/8 b Kkq -"}, Move(POS('a', 1), POS('a', 2), NOPOS, WROOK, WROOK, NOPC)},
    {{"8/2Q5/8/8/8/8/8/8 w Kkq -", "8/8/8/8/2Q5/8/8/8 b Kkq -"}, Move(POS('c', 7), POS('c', 4), NOPOS, WQUEEN, WQUEEN, NOPC)},
    {{"8/8/8/8/8/8/8/4K3 w KQkq -", "8/8/8/8/8/8/5K2/8 b kq -"}, Move(POS('e', 1), POS('f', 2), NOPOS, WKING, WKING, NOPC)},
    {{"8/3p4/8/8/8/8/8/8 b KQkq -", "8/8/3p4/8/8/8/8/8 w KQkq -"}, Move(POS('d', 7), POS('d', 6), NOPOS, BPAWN, BPAWN, NOPC)},
    {{"8/5p2/8/8/8/8/8/8 b KQkq -", "8/8/8/5p2/8/8/8/8 w KQkq f6"}, Move(POS('f', 7), POS('f', 5), NOPOS, BPAWN, BPAWN, NOPC)},
    {{"8/8/8/8/1n6/8/8/8 b KQkq -", "8/8/8/8/8/3n4/8/8 w KQkq -"}, Move(POS('b', 4), POS('d', 3), NOPOS, BKNIGHT, BKNIGHT, NOPC)},
    {{"8/3b4/8/8/8/8/8/8 b KQkq b3", "8/8/4b3/8/8/8/8/8 w KQkq -"}, Move(POS('d', 7), POS('e', 6), NOPOS, BBISHOP, BBISHOP, NOPC)},
    {{"7r/8/8/8/8/8/8/8 b KQkq -", "8/8/8/8/8/7r/8/8 w KQq -"}, Move(POS('h', 8), POS('h', 3), NOPOS, BROOK, BROOK, NOPC)},
    {{"r7/8/8/8/8/8/8/8 b KQkq -", "1r6/8/8/8/8/8/8/8 w KQk -"}, Move(POS('a', 8), POS('b', 8), NOPOS, BROOK, BROOK, NOPC)},
    {{"8/8/8/4q3/8/8/8/8 b Kq c3", "8/8/8/8/8/8/8/q7 w Kq -"}, Move(POS('e', 5), POS('a', 1), NOPOS, BQUEEN, BQUEEN, NOPC)},
    {{"4k3/8/8/8/8/8/8/8 b kq -", "3k4/8/8/8/8/8/8/8 w - -"}, Move(POS('e', 8), POS('d', 8), NOPOS, BKING, BKING, NOPC)}};

static map<vector<string>, Move> applyBasicCaptureCases =
   {{{"8/8/8/3p4/2P5/8/8/8 w KQkq -", "8/8/8/3P4/8/8/8/8 b KQkq -"}, Move(POS('c', 4), POS('d', 5), POS('d', 5), WPAWN, WPAWN, BPAWN)},
    {{"8/8/8/8/8/2b5/8/1N6 w KQkq f8", "8/8/8/8/8/2N5/8/8 b KQkq -"}, Move(POS('b', 1), POS('c', 3), POS('c', 3), WKNIGHT, WKNIGHT, BBISHOP)},
    {{"8/8/8/B7/8/2q5/8/8 w KQkq -", "8/8/8/8/8/2B5/8/8 b KQkq -"}, Move(POS('a', 5), POS('c', 3), POS('c', 3), WBISHOP, WBISHOP, BQUEEN)},
    {{"8/8/8/8/8/8/8/R1n5 w KQkq -", "8/8/8/8/8/8/8/2R5 b Kkq -"}, Move(POS('a', 1), POS('c', 1), POS('c', 1), WROOK, WROOK, BKNIGHT)},
    {{"8/8/8/2Q2r2/8/8/8/8 w k -", "8/8/8/5Q2/8/8/8/8 b k -"}, Move(POS('c', 5), POS('f', 5), POS('f', 5), WQUEEN, WQUEEN, BROOK)},
    {{"8/8/8/8/8/8/3n4/4K3 w KQ c6", "8/8/8/8/8/8/3K4/8 b - -"}, Move(POS('e', 1), POS('d', 2), POS('d', 2), WKING, WKING, BKNIGHT)},
    {{"8/8/8/8/8/8/3p4/2R5 b - g3", "8/8/8/8/8/8/8/2q5 w - -"}, Move(POS('d', 2), POS('c', 1), POS('c', 1), BPAWN, BQUEEN, WROOK)},
    {{"8/8/3n4/8/4B3/8/8/8 b Qkq -", "8/8/8/8/4n3/8/8/8 w Qkq -"}, Move(POS('d', 6), POS('e', 4), POS('e', 4), BKNIGHT, BKNIGHT, WBISHOP)},
    {{"8/b7/8/8/8/8/5P2/8 b Kkq -", "8/8/8/8/8/8/5b2/8 w Kkq -"}, Move(POS('a', 7), POS('f', 2), POS('f', 2), BBISHOP, BBISHOP, WPAWN)},
    {{"r1Q5/8/8/8/8/8/8/8 b Kq -", "2r5/8/8/8/8/8/8/8 w K -"}, Move(POS('a', 8), POS('c', 8), POS('c', 8), BROOK, BROOK, WQUEEN)},
    {{"8/8/8/8/8/8/5q2/5R2 b - b3", "8/8/8/8/8/8/8/5q2 w - -"}, Move(POS('f', 2), POS('f', 1), POS('f', 1), BQUEEN, BQUEEN, WROOK)},
    {{"4k3/3N4/8/8/8/8/8/8 b Qkq -", "8/3k4/8/8/8/8/8/8 w Q -"}, Move(POS('e', 8), POS('d', 7), POS('d', 7), BKING, BKING, WKNIGHT)}};

static map<vector<string>, Move> applyCastlingCases =
  {{{"8/8/8/8/8/8/8/4K2R w K -", "8/8/8/8/8/8/8/5RK1 b - -"}, Move(POS('e', 1), POS('g', 1), NOPOS, WKING, WKING, NOPC)},
   {{"8/8/8/8/8/8/8/R3K3 w Qkq -", "8/8/8/8/8/8/8/2KR4 b kq -"}, Move(POS('e', 1), POS('c', 1), NOPOS, WKING, WKING, NOPC)},
   {{"4k2r/8/8/8/8/8/8/8 b KQk -", "5rk1/8/8/8/8/8/8/8 w KQ -"}, Move(POS('e', 8), POS('g', 8), NOPOS, BKING, BKING, NOPC)},
   {{"r3k3/8/8/8/8/8/8/8 b q -", "2kr4/8/8/8/8/8/8/8 w - -"}, Move(POS('e', 8), POS('c', 8), NOPOS, BKING, BKING, NOPC)}};

static map<vector<string>, Move> applyEnPassantCases =
   {{{"8/8/8/5Pp1/8/8/8/8 w KQkq g6", "8/8/6P1/8/8/8/8/8 b KQkq -"}, Move(POS('f', 5), POS('g', 6), POS('g', 5), WPAWN, WPAWN, BPAWN)},
    {{"8/8/8/8/Pp6/8/8/8 b KQkq a3", "8/8/8/8/8/p7/8/8 w KQkq -"}, Move(POS('b', 4), POS('a', 3), POS('a', 4), BPAWN, BPAWN, WPAWN)}};

static map<vector<string>, Move> applyPromotionCases =
   {{{"8/3P4/8/8/8/8/8/8 w KQkq a6", "3Q4/8/8/8/8/8/8/8 b KQkq -"}, Move(POS('d', 7), POS('d', 8), NOPOS, WPAWN, WQUEEN, NOPC)},
    {{"8/1P6/8/8/8/8/8/8 w KQk d6", "1R6/8/8/8/8/8/8/8 b KQk -"}, Move(POS('b', 7), POS('b', 8), NOPOS, WPAWN, WROOK, NOPC)},
    {{"8/7P/8/8/8/8/8/8 w KQ g6", "7B/8/8/8/8/8/8/8 b KQ -"}, Move(POS('h', 7), POS('h', 8), NOPOS, WPAWN, WBISHOP, NOPC)},
    {{"8/5P2/8/8/8/8/8/8 w K -", "5N2/8/8/8/8/8/8/8 b K -"}, Move(POS('f', 7), POS('f', 8), NOPOS, WPAWN, WKNIGHT, NOPC)},
    {{"8/8/8/8/8/8/1p6/8 b Qkq b3", "8/8/8/8/8/8/8/1q6 w Qkq -"}, Move(POS('b', 2), POS('b', 1), NOPOS, BPAWN, BQUEEN, NOPC)},
    {{"8/8/8/8/8/8/3p4/8 b kq e3", "8/8/8/8/8/8/8/3r4 w kq -"}, Move(POS('d', 2), POS('d', 1), NOPOS, BPAWN, BROOK, NOPC)},
    {{"8/8/8/8/8/8/5p2/8 b q h3", "8/8/8/8/8/8/8/5b2 w q -"}, Move(POS('f', 2), POS('f', 1), NOPOS, BPAWN, BBISHOP, NOPC)},
    {{"8/8/8/8/8/8/p7/8 b - -", "8/8/8/8/8/8/8/n7 w - -"}, Move(POS('a', 2), POS('a', 1), NOPOS, BPAWN, BKNIGHT, NOPC)}};

TEST(TestBoard, ValConstruct) {
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        Board b(it->first.c_str());
        for (int i = 0; i < 8; ++i) {
            EXPECT_EQ(b.ranks_[i], it->second[i]) << "diff at rank " << i << " on build" << endl;
        }
        EXPECT_EQ(FLAGS_WKCASTLE(b.flags_), it->second[8]) << "wrong white kingside castling priv bit" << endl;
        EXPECT_EQ(FLAGS_WQCASTLE(b.flags_), it->second[9]) << "wrong white queenside castling priv bit" << endl;
        EXPECT_EQ(FLAGS_BKCASTLE(b.flags_), it->second[10]) << "wrong black kingside castling priv bit" << endl;
        EXPECT_EQ(FLAGS_BQCASTLE(b.flags_), it->second[11]) << "wrong black queenside castling priv bit" << endl;
        EXPECT_EQ(FLAGS_WPLAYER(b.flags_), it->second[12]) << "wrong player bit" << endl;
        EXPECT_NE(FLAGS_BPLAYER(b.flags_), it->second[12]) << "both white and black player ???" << endl;
        EXPECT_EQ(FLAGS_EP(b.flags_), it->second[13]) << "wrong ep position" << endl;
    }
}

TEST(TestBoard, CpyConstruct) {
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        Board b1(it->first.c_str());
        Board b2(b1);
        for (int i = 0; i < 8; ++i) {
            EXPECT_EQ(b2.ranks_[i], b1.ranks_[i]) << "diff at rank " << i << " on cpy construct" << endl;
        }
        EXPECT_EQ(b2.flags_, b1.flags_) << "diff flags on cpy construct" << endl;
    }
}

TEST(TestBoard, AssignOp) {
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        Board b1(it->first.c_str());
        Board b2 = b1;
        for (int i = 0; i < 8; ++i) {
            EXPECT_EQ(b2.ranks_[i], b1.ranks_[i]) << "diff at rank " << i << " on assignment op" << endl;
        }
        EXPECT_EQ(b2.flags_, b1.flags_) << "diff flags on assignment op" << endl;
    }
}

// integration
TEST(TestBoard, CpyAssign) {
    Board b1;
    Board b2(b1);
    Board b3 = b1;
    b1 = Board(FEN_RAND_8);
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(b3.ranks_[i], b2.ranks_[i]) << "diff at rank " << i;
    }
    EXPECT_EQ(b3.flags_, b2.flags_) << "diff flags";
    for (int i = 0; i < 8; ++i) {
        if (i == 3 || i == 4) {
            EXPECT_EQ(b2.ranks_[i], b1.ranks_[i]) << "diff at rank " << i;
        } else {
            EXPECT_NE(b2.ranks_[i], b1.ranks_[i]) << "eq at rank " << i;
        }
    }
    EXPECT_NE(b2.flags_, b1.flags_) << "diff flags";
}

TEST(TestBoard, GenFen) {
    for (auto it = buildCases.begin(); it != buildCases.end(); ++it) {
        Board b(it->first.c_str());
        EXPECT_EQ(it->first, b.toFen()) << "diff fen post on gen post construct";
    }
}

TEST(TestBoard, PrintOp) {
    for (auto it = printCases.begin(); it != printCases.end(); ++it) {
        // expected
        string expect(PRINT_TOP_PAD);
        for (int i = 0; i < 8; ++i) {
            expect.append(it->second[i]);
        }
        expect.append(PRINT_BOT_PAD);
        // actual
        Board b(it->first.c_str());
        std::ostringstream os;
        os << b;
        EXPECT_EQ(os.str(), expect);
    }
}

TEST(TestBoard, ApplyBasic) {
    for (auto it = applyBasicMoveCases.begin(); it != applyBasicMoveCases.end(); ++it) {
        Board b(it->first[0].c_str());
        b.applyMove(it->second);
        EXPECT_EQ(b.toFen(), it->first[1]) << "diff at move " << it->second << endl;
    }
}

TEST(TestBoard, ApplyCaptureBasic) {
    for (auto it = applyBasicCaptureCases.begin(); it != applyBasicCaptureCases.end(); ++it) {
        Board b(it->first[0].c_str());
        b.applyMove(it->second);
        EXPECT_EQ(b.toFen(), it->first[1]) << "diff at move " << it->second << endl;
    }
}

TEST(TestBoard, ApplyCastling) {
    for (auto it = applyCastlingCases.begin(); it != applyCastlingCases.end(); ++it) {
        Board b(it->first[0].c_str());
        b.applyMove(it->second);
        EXPECT_EQ(b.toFen(), it->first[1]) << "diff at move " << it->second << endl;
    }
}

TEST(TestBoard, ApplyEnPassant) {
    for (auto it = applyEnPassantCases.begin(); it != applyEnPassantCases.end(); ++it) {
        Board b(it->first[0].c_str());
        b.applyMove(it->second);
        EXPECT_EQ(b.toFen(), it->first[1]) << "diff at move " << it->second << endl;
    }
}

TEST(TestBoard, ApplyPromotion) {
    for (auto it = applyPromotionCases.begin(); it != applyPromotionCases.end(); ++it) {
        Board b(it->first[0].c_str());
        b.applyMove(it->second);
        EXPECT_EQ(b.toFen(), it->first[1]) << "diff at move " << it->second << endl;
    }
}
