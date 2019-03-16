#include <gtest/gtest.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#include "../game/move.h"

using std::cout;
using std::endl;
using std::vector;
using std::map;
using std::string;
using game::Move;

//     frompos  |    topos   |    killpos  |  frompc | topc | killpc |  isCap  |  isEP | isProm | castle    |   op<<     |    algNot
static map<vector<int>, vector<string>> rawCases =
   {{{POS('e', 2), POS('e', 4), NOPOS,       WPAWN,   WPAWN,   NOPC,    false,   false,   false,   0       }, {"e2e4",       "Pe2e4"}       },  // w move
    {{POS('b', 1), POS('c', 3), POS('c', 3), WKNIGHT, WKNIGHT, BBISHOP, true,    false,   false,   0       }, {"b1xc3",      "Nb1xbc3"}     },  // w cap
    {{POS('f', 6), POS('c', 3), NOPOS,       BBISHOP, BBISHOP, NOPC,    false,   false,   false,   0       }, {"f6c3",       "bf6c3"}       },  // b move
    {{POS('a', 5), POS('d', 5), POS('d', 5), BROOK,   BROOK,   WPAWN,   true,    false,   false,   0       }, {"a5xd5",      "ra5xPd5"}     },  // b cap
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

TEST(TestMove, ValConstruct) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m.frompos_, it->first[0]) << "diff at " << it->second[0];
        EXPECT_EQ(m.topos_, it->first[1]) << "diff at " << it->second[0];
        EXPECT_EQ(m.killpos_, it->first[2]) << "diff at " << it->second[0];
        EXPECT_EQ(m.frompc_, it->first[3]) << "diff at " << it->second[0];
        EXPECT_EQ(m.topc_, it->first[4]) << "diff at " << it->second[0];
        EXPECT_EQ(m.killpc_, it->first[5]) << "diff at " << it->second[0];
    }
}

TEST(TestMove, AlgNotConstruct) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m1(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        Move m2(it->second[1].c_str());
        EXPECT_EQ(m1.frompos_, m2.frompos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.topos_, m2.topos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.killpos_, m2.killpos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.frompc_, m2.frompc_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.topc_, m2.topc_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.killpc_, m2.killpc_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.isCapture(), m2.isCapture()) << "diff at " << it->second[0];
        EXPECT_EQ(m1.isEnPassant(), m2.isEnPassant()) << "diff at " << it->second[0];
        EXPECT_EQ(m1.isPromotion(), m2.isPromotion()) << "diff at " << it->second[0];
        EXPECT_EQ(m1.castleType(), m2.castleType()) << "diff at " << it->second[0];
        std::ostringstream os1;
        std::ostringstream os2;
        os1 << m1;
        os2 << m2;
        EXPECT_EQ(os1.str(), os2.str()) << "diff at " << it->second[0];
    }
}

TEST(TestMove, Assign) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m1(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        Move m2(NOPOS, NOPOS, NOPOS, NOPC, NOPC, NOPC);
        m2 = m1;
        EXPECT_EQ(m1.frompos_, m2.frompos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.topos_, m2.topos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.killpos_, m2.killpos_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.frompc_, m2.frompc_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.topc_, m2.topc_) << "diff at " << it->second[0];
        EXPECT_EQ(m1.killpc_, m2.killpc_) << "diff at " << it->second[0];
    }
}

TEST(TestMove, IsCapture) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m.isCapture(), it->first[6]) << "diff at " << it->second[0];
    }
}

TEST(TestMove, IsEnPassant) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m.isEnPassant(), it->first[7]) << "diff at " << it->second[0];
    }
}

TEST(TestMove, IsPromotion) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m.isPromotion(), it->first[8]) << "diff at " << it->second[0];
    }
}

TEST(TestMove, CastleType) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        EXPECT_EQ(m.castleType(), it->first[9]) << "diff at " << it->second[0];
    }
}

TEST(TestMove, Print) {
    for (auto it = rawCases.begin(); it != rawCases.end(); ++it) {
        Move m(it->first[0], it->first[1], it->first[2], it->first[3], it->first[4], it->first[5]);
        std::ostringstream os;
        os << m;
        EXPECT_EQ(os.str(), it->second[0]) << "diff at " << it->second[0];
    }
}
