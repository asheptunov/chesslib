extern "C" {
#include "move.h"
}

#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::string;

//     frompos  |    topos   |    killpos  |  frompc | topc | killpc |  isCap  |  isEP | isProm | castle    |   op<<     |    algNot
static map<vector<int>, vector<string>> rawCases =
   {{{POS('e', 2), POS('e', 4), NOPOS,       WPAWN,   WPAWN,   NOPC,    false,   false,   false,   0       }, {"e2e4",       "Pe2e4"}       },  // w move
    {{POS('b', 1), POS('c', 3), POS('c', 3), WKNIGHT, WKNIGHT, BBISHOP, true,    false,   false,   0       }, {"b1xc3",      "Nb1xbc3"}     },  // w cap
    {{POS('f', 6), POS('c', 3), NOPOS,       BBISHOP, BBISHOP, NOPC,    false,   false,   false,   0       }, {"f6c3",       "bf6c3"}       },  // b move
    {{POS('a', 5), POS('d', 5), POS('d', 5), BROOK,   BROOK,   WPAWN,   true,    false,   false,   0       }, {"a5xd5",      "ra5xPd5"}     },  // b cap
    {{POS('b', 8), POS('c', 8), NOPOS,       BKING,   BKING,   NOPC,    false,   false,   false,   0       }, {"b8c8",       "kb8c8"}       },  // b k move
    {{POS('e', 1), POS('g', 1), NOPOS,       WKING,   WKING,   NOPC,    false,   false,   false,   WKCASTLE}, {"0-0",        "Ke1g1"}       },  // w k castle
    {{POS('e', 1), POS('c', 1), NOPOS,       WKING,   WKING,   NOPC,    false,   false,   false,   WQCASTLE}, {"0-0-0",      "Ke1c1"}       },  // w q castle
    {{POS('e', 8), POS('g', 8), NOPOS,       BKING,   BKING,   NOPC,    false,   false,   false,   BKCASTLE}, {"0-0",        "ke8g8"}       },  // b k castle
    {{POS('e', 8), POS('c', 8), NOPOS,       BKING,   BKING,   NOPC,    false,   false,   false,   BQCASTLE}, {"0-0-0",      "ke8c8"}       },  // b q castle
    {{POS('f', 7), POS('f', 8), NOPOS,       WPAWN,   WQUEEN,  NOPC,    false,   false,   true,    0       }, {"f7f8Q",      "Pf7f8Q"}      },  // w p->q promote
    {{POS('c', 7), POS('b', 8), POS('b', 8), WPAWN,   WROOK,   BKNIGHT, true,    false,   true,    0       }, {"c7xb8R",     "Pc7xnb8R"}    },  // w p->r cap promote
    {{POS('d', 2), POS('d', 1), NOPOS,       BPAWN,   BBISHOP, NOPC,    false,   false,   true,    0       }, {"d2d1b",      "pd2d1b"}      },  // b p->b promote
    {{POS('b', 2), POS('a', 1), POS('a', 1), BPAWN,   BKNIGHT, WROOK,   true,    false,   true,    0       }, {"b2xa1n",     "pb2xRa1n"}    },  // b p->k cap promote
    {{POS('d', 5), POS('e', 6), POS('e', 5), WPAWN,   WPAWN,   BPAWN,   true,    true,    false,   0       }, {"d5xe6e.p.",  "Pd5xpe6e.p."} },  // w ep cap
    {{POS('g', 4), POS('f', 3), POS('f', 4), BPAWN,   BPAWN,   WPAWN,   true,    true,    false,   0       }, {"g4xf3e.p.",  "pg4xPf3e.p."} }};  // b ep cap

TEST(MoveTest, ValConstruct) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(MVFROMPOS(m), it->first[0]) << "diff at " << it->second[0];
        EXPECT_EQ(MVTOPOS(m), it->first[1]) << "diff at " << it->second[0];
        EXPECT_EQ(MVKILLPOS(m), it->first[2]) << "diff at " << it->second[0];
        EXPECT_EQ(MVFROMPC(m), it->first[3]) << "diff at " << it->second[0];
        EXPECT_EQ(MVTOPC(m), it->first[4]) << "diff at " << it->second[0];
        EXPECT_EQ(MVKILLPC(m), it->first[5]) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m->frompos, it->first[0]) << "diff at " << it->second[0];
        EXPECT_EQ(m->topos, it->first[1]) << "diff at " << it->second[0];
        EXPECT_EQ(m->killpos, it->first[2]) << "diff at " << it->second[0];
        EXPECT_EQ(m->frompc, it->first[3]) << "diff at " << it->second[0];
        EXPECT_EQ(m->topc, it->first[4]) << "diff at " << it->second[0];
        EXPECT_EQ(m->killpc, it->first[5]) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, AlgNotConstruct) {
    char str1[16];
    char str2[16];
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m1 = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        move_t m2 = move_make_algnot(it->second[1].c_str());
        EXPECT_EQ(MVFROMPOS(m1), MVFROMPOS(m2)) << "diff at " << it->second[0];
        EXPECT_EQ(MVTOPOS(m1),   MVTOPOS(m2))   << "diff at " << it->second[0];
        EXPECT_EQ(MVKILLPOS(m1), MVKILLPOS(m2)) << "diff at " << it->second[0];
        EXPECT_EQ(MVFROMPC(m1),  MVFROMPC(m2))  << "diff at " << it->second[0];
        EXPECT_EQ(MVTOPC(m1),    MVTOPC(m2))    << "diff at " << it->second[0];
        EXPECT_EQ(MVKILLPC(m1),  MVKILLPC(m2))  << "diff at " << it->second[0];
        EXPECT_EQ(move_is_cap(m1),    move_is_cap(m2))    << "diff at " << it->second[0];
        EXPECT_EQ(move_is_ep(m1),     move_is_ep(m2))     << "diff at " << it->second[0];
        EXPECT_EQ(move_is_promo(m1),  move_is_promo(m2))  << "diff at " << it->second[0];
        EXPECT_EQ(move_is_castle(m1), move_is_castle(m2)) << "diff at " << it->second[0];
        strcpy(str1, move_str(m1));
        strcpy(str2, move_str(m2));
        EXPECT_EQ(strcmp(str1, str2), 0) << "diff at " << it->second[0];
#else
        move_t *m1 = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        move_t *m2 = move_make_algnot(it->second[1].c_str());
        EXPECT_EQ(m1->frompos, m2->frompos) << "diff at " << it->second[0];
        EXPECT_EQ(m1->topos, m2->topos) << "diff at " << it->second[0];
        EXPECT_EQ(m1->killpos, m2->killpos) << "diff at " << it->second[0];
        EXPECT_EQ(m1->frompc, m2->frompc) << "diff at " << it->second[0];
        EXPECT_EQ(m1->topc, m2->topc) << "diff at " << it->second[0];
        EXPECT_EQ(m1->killpc, m2->killpc) << "diff at " << it->second[0];
        EXPECT_EQ(move_is_cap(m1), move_is_cap(m2)) << "diff at " << it->second[0];
        EXPECT_EQ(move_is_ep(m1), move_is_ep(m2)) << "diff at " << it->second[0];
        EXPECT_EQ(move_is_promo(m1), move_is_promo(m2)) << "diff at " << it->second[0];
        EXPECT_EQ(move_is_castle(m1), move_is_castle(m2)) << "diff at " << it->second[0];
        strcpy(str1, move_str(m1));
        strcpy(str2, move_str(m2));
        EXPECT_EQ(strcmp(str1, str2), 0) << "diff at " << it->second[0];
        move_free(m1);
        move_free(m2);
#endif
    }
}

TEST(MoveTest, IsCapture) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_cap(m), it->first[6]) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_cap(m), it->first[6]) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, IsEnPassant) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_ep(m), it->first[7]) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_ep(m), it->first[7]) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, IsPromotion) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_promo(m), it->first[8]) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_promo(m), it->first[8]) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, CastleType) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_castle(m), it->first[9]) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(move_is_castle(m), it->first[9]) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, Print) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
#ifdef CHESSLIB_QWORD_MOVE
        move_t m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        char *str = move_str(m);
        EXPECT_EQ(strcmp(str, it->second[0].c_str()), 0) << "diff at " << it->second[0];
#else
        move_t *m = move_make(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        char *str = move_str(m);
        EXPECT_EQ(strcmp(str, it->second[0].c_str()), 0) << "diff at " << it->second[0];
        move_free(m);
#endif
    }
}

TEST(MoveTest, LessThan) {
#ifdef CHESSLIB_QWORD_MOVE
    move_t m1 = move_make_algnot("Ke1e2");
    move_t m2 = move_make_algnot("ke1e2");
#else
    move_t *m1 = move_make_algnot("Ke1e2");
    move_t *m2 = move_make_algnot("ke1e2");
#endif
    ASSERT_TRUE(move_cmp(m1, m2) < 0);
    ASSERT_FALSE(move_cmp(m2, m1) < 0);
#ifndef CHESSLIB_QWORD_MOVE
    move_free(m1);
    move_free(m2);
#endif

    m1 = move_make_algnot("Qe1e2");
    m2 = move_make_algnot("Ke1e2");
    ASSERT_TRUE(move_cmp(m1, m2) < 0);
    ASSERT_FALSE(move_cmp(m2, m1) < 0);
#ifndef CHESSLIB_QWORD_MOVE
    move_free(m1);
    move_free(m2);
#endif

    m1 = move_make_algnot("Pe2e3");
    m2 = move_make_algnot("Pe2e4");
    ASSERT_TRUE(move_cmp(m1, m2) < 0);
    ASSERT_FALSE(move_cmp(m2, m1) < 0);
#ifndef CHESSLIB_QWORD_MOVE
    move_free(m1);
    move_free(m2);
#endif

    m1 = move_make_algnot("rb2b5");
    m2 = move_make_algnot("rb3b5");
    ASSERT_TRUE(move_cmp(m1, m2) < 0);
    ASSERT_FALSE(move_cmp(m2, m1) < 0);
#ifndef CHESSLIB_QWORD_MOVE
    move_free(m1);
    move_free(m2);
#endif

    m1 = move_make_algnot("Na1b3");
    m2 = move_make_algnot("Na1b3");
    ASSERT_FALSE(move_cmp(m1, m2));
#ifndef CHESSLIB_QWORD_MOVE
    move_free(m1);
    move_free(m2);
#endif
}
