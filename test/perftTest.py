import os
import sys
import time
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
import src
from src import chess

def search(board:chess.Board, depth:int):
  if depth <= 0:
    # pseudo-leaf (hit depth limit)
    # this is counted by perft
    return 1
  moves = board.get_moves()
  if len(moves) == 0:
    # this isn't counted by perft
    return 0
  ct = 0
  for move in moves:
    future_board = board.apply_move(move)
    ct += search(future_board, depth - 1)
  return ct

def nps(board, depth, samples):
  nps = [0] * samples
  for i in range(samples):
    start_time = time.time()
    move_ct = search(board, depth)
    end_time = time.time()
    elapsed = end_time - start_time
    nps[i] = move_ct / elapsed
  return sum(nps) / len(nps)

class TestPerftMovegenCounts(unittest.TestCase):
  '''
  Counts given at https://www.chessprogramming.org/Perft_Results
  '''
  def __init__(self, *args, **kwargs):
    super(TestPerftMovegenCounts, self).__init__(*args, **kwargs)
    self.thresh = 1_000_000

  def verify_perft_n(self, fen, thresh, expected_counts):
    board = chess.Board.from_fen(fen)
    for i in range(len(expected_counts)):
      if expected_counts[i] <= thresh:
        self.assertEqual(search(board, i), expected_counts[i])

  def test_correctness_perft_1(self):
    self.verify_perft_n(chess.STARTING_FEN, self.thresh, [1, 20, 400, 8_902, 197_281, 4_865_609, 119_060_324, 3_195_901_860])

  def test_correctness_perft_2(self):
    self.verify_perft_n('r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -', self.thresh, [1, 48, 2_039, 97_862, 4_085_603, 193_690_690, 8_031_647_685])
  
  def test_correctness_perft_3(self):
    self.verify_perft_n('8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -', self.thresh, [1, 14, 191, 2_812, 43_238, 674_624, 11_030_083, 178_633_661, 3_009_794_393])
  
  def test_correctness_perft_4(self):
    self.verify_perft_n('r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -', self.thresh, [1, 6, 264, 9_467, 422_333, 15_833_292, 706_045_033])
  
  def test_correctness_perft_5(self):
    self.verify_perft_n('rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -', self.thresh, [1, 44, 1_486, 62_379, 2_103_487, 89_941_194])
  
  def test_correctness_perft_6(self):
    self.verify_perft_n('r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -', self.thresh, [1, 46, 2_079, 89_890, 3_894_594, 164_075_551, 6_923_051_137])

class TestPerftSpeed(unittest.TestCase):
  def verify_perft_n(self, fen, depth, samples, expected_nps):
    board = chess.Board.from_fen(fen)
    self.assertGreater(nps(board, depth, samples), expected_nps)

  def test_speed_perft_2(self):
    self.verify_perft_n('r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -', 3, 3, 10_000)

if __name__ == '__main__':
  unittest.main()
