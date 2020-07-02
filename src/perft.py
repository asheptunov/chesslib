import ctypes

from . import minimax
from . import chess

def search(board, depth):
  if depth <= 0:
    # pseudo-leaf (hit depth limit)
    return 1
  moves = chess.board_get_moves(board)
  if moves.contents.len == 0:
    chess.alst_free(moves, ctypes.cast(chess.move_free, chess.alst_free_func_type))
    return 0
  ct = 0
  for i in range(moves.contents.len):
    move = ctypes.cast(chess.alst_get(moves, i), ctypes.POINTER(chess.MOVE))
    future_board = chess.board_copy(board)
    chess.board_apply_move(future_board, move)
    ct += search(future_board, depth - 1)
    chess.board_free(future_board)
  chess.alst_free(moves, ctypes.cast(chess.move_free, chess.alst_free_func_type))
  return ct
