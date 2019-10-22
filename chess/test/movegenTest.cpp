#include <gtest/gtest.h>
#include <cstdlib>
#include <map>
#include <cstring>
#include <string>
#include <algorithm>

#include "board.h"
#include "move.h"

using game::Board;
using game::Move;
using std::map;
using std::string;
using std::vector;
using std::strcpy;

// move list is tsv (from excel)
static map <string, string> genCases =
   {{STARTING_BOARD,                                                         "Pa2a3	Pa2a4	Pb2b3	Pb2b4	Pc2c3	Pc2c4	Pd2d3	Pd2d4	Pe2e3	Pe2e4	Pf2f3	Pf2f4	Pg2g3	Pg2g4	Ph2h3	Ph2h4	Nb1c3	Nb1a3	Ng1h3	Ng1f3"},
    {"r1bqk2r/p2nppbp/2pp1npB/1p6/3PP3/2N2P2/PPPQN1PP/R3KB1R b KQkq -",      "ke8g8	ke8f8	pa7a6	pa7a5	pe7e6	pe7e5	pc6c5	pd6d5	pg6g5	pb5b4	ra8b8	bc8b7	bc8a6	qd8c7	qd8b6	qd8a5	rh8g8	rh8f8	nd7c5	nd7b6	nd7f8	nd7b8	nd7e5	bg7xBh6	bg7f8	nf6xPe4	nf6g8	nf6d5	nf6h5	nf6g4"},
    {"r2qk2r/pb1n1p1p/2pp1npQ/1p2p3/3PP3/P1N2P2/1PP1N1PP/R3KB1R w KQkq e6",  "Ke1c1	Ke1d1	Ke1f2	Ke1d2	Pd4d5	Pd4xpe5	Pa3a4	Pf3f4	Pb2b3	Pb2b4	Pg2g3	Pg2g4	Ph2h3	Ph2h4	Qh6xpg6	Qh6g5	Qh6f4	Qh6e3	Qh6d2	Qh6c1	Qh6h5	Qh6h4	Qh6h3	Qh6xph6	Qh6g7	Qh6f8	Nc3b1	Nc3d5	Nc3a2	Nc3a4	Nc3d1	Nc3xpb5	Ne2f4	Ne2c1	Ne2g3	Ne2g1	Ra1b1	Ra1c1	Ra1d1	Ra1a2	Rh1g1"},
    {"r3k2r/1b1nqp1p/p1pp1npQ/1p2p3/3PP3/P1N2P2/1PP3PP/1KNR1B1R b kq -",     "ke8c8	ke8d8	pa6a5	pc6c5	pd6d5	pg6g5	pb5b4	pe5xPd4	ra8b8	ra8c8	ra8d8	ra8a7	rh8g8	rh8f8	bb7c8	nd7c5	nd7b6	nd7f8	nd7b8	qe7e6	qe7f8	qe7d8	nf6xPe4	nf6g8	nf6d5	nf6h5	nf6g4"},
    {"1n1Rkb1r/p4ppp/4q3/4p1B1/4P3/8/PPP2PPP/2K5 b k -",                     ""},
    {"r1bqk2r/p2n1ppp/2pbpn2/1p6/3P4/2NBPN2/PP3PPP/R1BQK2R w KQkq -",        "Ke1g1	Ke1f1	Ke1e2	Ke1d2	Pd4d5	Pe3e4	Pa2a3	Pa2a4	Pb2b3	Pb2b4	Pg2g3	Pg2g4	Ph2h3	Ph2h4	Nc3b1	Nc3d5	Nc3e4	Nc3e2	Nc3a4	Nc3xpb5	Bd3c2	Bd3b1	Bd3e4	Bd3f5	Bd3g6	Bd3xph7	Bd3e2	Bd3f1	Bd3c4	Bd3xpb5	Nf3g5	Nf3d2	Nf3h4	Nf3g1	Nf3e5	Ra1b1	Bc1d2	Qd1e2	Qd1d2	Qd1c2	Qd1b3	Qd1a4	Rh1g1	Rh1f1"},
    {"r1bqk2r/p2n1ppp/2pbpn2/1p6/3P4/2NBPN2/PP3PPP/R1BQ1RK1 b kq -",         "ke8g8	ke8f8	ke8e7	pa7a6	pa7a5	pg7g6	pg7g5	ph7h6	pgh7h5	pc6c5	pe6e5	pb5b4	ra8b8	bc8b7	bc8a6	qd8c7	qd8b6	qd8a5	qd8e7	rh8g8	rh8f8	nd7c5	nd7b6	nd7f8	nd7b8	nd7e5	bd6c5	bd6b4	bd6a3	bd6e7	bd6f8	bd6e5	bd6f4	bd6g3	bd6xPh2	bd6c7	bd6b8	nf6e4	nf6g8	nf6d5	nf6h5	nf6g4"},
    {"2rq1rk1/pb3ppN/4p3/1p2n3/3b1PB1/P1N5/1PQ3PP/R1B2RK1 w - -",            "Kg1h1	Qc2f2	Bc1e3	Rf1f2"},
    {"2b1rrk1/2pR1p2/1pq1pQp1/p3P2p/P1PR1K1P/5N2/2P2PP1/8 w - -",            "Kf4e3	Kf4g5	Kf4g3	Pc4c5	Pc2c3	Pg2g3	Pg2g4	Rd7e7	Rd7xpf7	Rd7xpc7	Rd7d6	Rd7d5	Rd7d8	Qf6xpg6	Qf6xpe6	Qf6f5	Qf6g5	Qf6g7	Qf6h8	Qf6xpf7	Qf6e7	Qf6d8	Rd4e4	Rd4d3	Rd4d2	Rd4d1	Rd4d5	Rd4d6	Nf3e1	Nf3g5	Nf3d2	Nf3h2	Nf3g1"},
    {"r2q1rk1/pb1p1ppp/1pn1pn2/2p5/2PP4/P1PBPP2/4N1PP/R1BQ1RK1 b - -",       "kg8h8	pa7a6	pa7a5	pd7d6	pd7d5	pg7g6	pg7g5	ph7h6	ph7h5	pb6b5	pe6e5	pc5xPd4	ra8b8	ra8c8	qd8e8	qd8c8	qd8b8	qd8c7	qd8e7	rf8e8	bb7a6	bb7c8	nc6b4	nc6a5	nc6e7	nc6e5	nc6xPd4	nc6b8	nf6e4	nf6d5	nf6h5	nf6g4	nf6e8"},
    {"r1bqk2r/ppp2ppp/2n2n2/2bpP3/2Bp4/2P2N2/PP3PPP/RNBQK2R w KQkq d6",      "Qd1c2	Qd1b3	Qd1a4	Qd1e2	Qd1d2	Qd1d3	Qd1xpd4	Rh1g1	Rh1f1	Bc1d2	Bc1e3	Bc1f4	Bc1g5	Bc1h6	Bc4b5	Bc4a6	Bc4xpd5	Bc4d3	Bc4e2	Bc4f1	Bc4b3	Nb1a3	Nb1d2	Nf3xpd4	Nf3g5	Nf3h4	Nf3d2	Nf3g1	a2a3	a2a4	b2b3	b2b4	g2g3	g2g4	h2h3	h2h4	Pc3xpd4	e5e6	Pe5xnf6	Pe5xpd6e.p.	Ke1g1	Ke1f1	Ke1d2	Ke1e2"},
    {"8/8/P7/8/4Pp2/8/k4b2/3K4 b - e3",                                      "bf2e1	bf2g1	bf2g3	bf2h4	bf2e3	bf2d4	bf2c5	bf2b6	bf2a7	pf4f3	pf4xPe3e.p.	ka2b2	ka2a1	ka2b1	ka2a3	ka2b3"},
    {"4k3/8/8/8/8/4r3/8/R3K2R w KQ -",                                       "Ke1f1	Ke1d1	Ke1d2	Ke1f2"},
    {"1k6/8/8/8/8/8/2p5/1R2K3 b - -",                                        "kb8c8	kb8a8	kb8a7	kb8c7	pc2xRb1q	pc2xRb1n	pc2xRb1r	pc2xRb1b"}};

TEST(TestBoardMoveGen, MoveList) {
   for (auto it = genCases.begin(); it != genCases.end(); ++it) {
      char moves[it->second.size()];
      strcpy(moves, it->second.c_str());
      vector<Move> expect;
      char *move = strtok(moves, "\t");
      while (move) {
         expect.push_back(move);
         move = strtok(NULL, "\t");
      }
      Board b(it->first.c_str());
      vector<Move> actual = b.generateMoves();
      EXPECT_EQ(actual.size(), expect.size()) << "diff moves list size";
      if (actual.size() != expect.size() || actual.size() == 0) continue;
      sort(actual.begin(), actual.end());
      sort(expect.begin(), expect.end());
      Move *m1;
      Move *m2;
      for (unsigned int i = 0; i < actual.size(); ++i) {
         m1 = &actual[i];
         m2 = &expect[i];
         EXPECT_EQ(strcmp(m1->algNot().c_str(), m2->algNot().c_str()), 0) << "move diff got " << m1->algNot() << " but expected " << m2->algNot();
         // EXPECT_EQ(m1->frompos_, m2->frompos_);
         // EXPECT_EQ(m1->frompc_, m2->frompc_);
         // EXPECT_EQ(m1->topos_, m2->topos_);
         // EXPECT_EQ(m1->topc_, m2->topc_);
         // EXPECT_EQ(m1->killpos_, m2->killpos_);
         // EXPECT_EQ(m1->killpc_, m2->killpc_);
      }
   }
}
