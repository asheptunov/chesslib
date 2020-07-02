import sys
import time
import unittest
from ctypes import *

from src import chess
from src import perft

thresh = 100_000

class TestPerftMovegenCounts(unittest.TestCase):
  '''
  Counts given at https://www.chessprogramming.org/Perft_Results
  '''

  def verify_perft_n(self, fen, expected_counts):
    board = chess.board_make(fen)
    for i in range(len(expected_counts)):
      if expected_counts[i] <= thresh:
        self.assertEqual(perft.search(board, i), expected_counts[i])
    chess.board_free(board)

  def test_perft_1(self):
    self.verify_perft_n('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -', [1, 20, 400, 8_902, 197_281, 4_865_609, 119_060_324, 3_195_901_860])

  def test_perft_2(self):
    self.verify_perft_n('r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -', [1, 48, 2_039, 97_862, 4_085_603, 193_690_690, 8_031_647_685])
  
  def test_perft_3(self):
    self.verify_perft_n('8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -', [1, 14, 191, 2_812, 43_238, 674_624, 11_030_083, 178_633_661, 3_009_794_393])
  
  def test_perft_4(self):
    self.verify_perft_n('r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -', [1, 6, 264, 9_467, 422_333, 15_833_292, 706_045_033])
  
  def test_perft_5(self):
    self.verify_perft_n('rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -', [1, 44, 1_486, 62_379, 2_103_487, 89_941_194])
  
  def test_perft_6(self):
    self.verify_perft_n('r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -', [1, 46, 2_079, 89_890, 3_894_594, 164_075_551, 6_923_051_137])

# class TestPerftNPS(unittest.TestCase):

#   def test_nps_perft_2_depth_4(self):
#     board = chess.board_make('r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -')
#     for i in range(10):
#       # search
#       start_time = time.time()
#       move_ct = perft.search(board, 4)
#       end_time = time.time()
#       elapsed = end_time - start_time

#       # verify correctness
#       self.assertEqual(move_ct, 4_085_603)

#       # report resulting nps
#       print('~%.2f nps' % round(move_ct / elapsed))
#     chess.board_free(board)

if __name__ == '__main__':
  unittest.main()
