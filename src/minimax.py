import os
import ctypes

import chess

WPAWN = 0
WKNIGHT = 1
WBISHOP = 2
WROOK = 3
WQUEEN = 4

def evaluate(board):
  '''
  returns a value for the board based on a simple evaluation function
  '''
  ret = 0
  player = not not ((board.contents.flags) & 0b10000)  # True if black, False if white
  for rk in range(0, 8):
    for offs in range(0, 8):
      pc = (board.contents.ranks[rk] >> (offs * 4)) & 0xf
      # print(pc)
      if (pc / 6) == player:
        ret += pcval(pc)
      else:
        ret -= pcval(pc)
  return ret

def pcval(pc):
  '''
  returns the approximate value of a piece
  '''
  pc = pc / 6
  if pc == WPAWN:
    return 1
  if pc == WKNIGHT or pc == WBISHOP:
    return 3
  if pc == WROOK:
    return 5
  if pc == WQUEEN:
    return 9
  return 0


def minimax(evaluate, board, depth):
  '''
  performs minimax search from the current board position, exploring down to a given depth;
  returns the optimal move from the current position (None if there are no moves, or depth is 0) and the value of the board when applying that move
  '''
  if depth <= 0:
    return (None, evaluate(board))

  moves = chess.board_get_moves(board)

  if moves.contents.len == 0:
    # active player has no moves; loss or stalemate
    chess.alst_free(moves, ctypes.cast(chess.move_free, chess.alst_free_func_type))
    return (None, -100_000 - depth)

  max_utility_move = None
  max_utility = -float('inf')
  for i in range(moves.contents.len):
    move = ctypes.cast(chess.alst_get(moves, i), ctypes.POINTER(chess.MOVE))
    future_board = chess.board_copy(board)
    chess.board_apply_move(future_board, move)
    opp_best_move, opp_utility = minimax(evaluate, future_board, depth - 1)
    chess.move_free(opp_best_move)
    chess.board_free(future_board)

    utility = -opp_utility  # negate since this is the opponent's maximal utility, which is our minimal utility
    if (utility > max_utility):
      max_utility = utility
      max_utility_move = move

  max_utility_move = chess.move_cpy(max_utility_move)  # copy it so we don't lose the data when we free the alst below
  chess.alst_free(moves, ctypes.cast(chess.move_free, chess.alst_free_func_type))
  return (max_utility_move, max_utility)

# use for debugging
if __name__ == '__main__':
    board = chess.board_make('rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -')
    depth = 5
    move, utility = minimax(evaluate, board, depth)
    print(chess.board_to_tui(board))
    print('searched to depth %d' % depth)
    print('best move %s' % chess.move_str(move))
    print('utility %d' % utility)

    # cleanup
    chess.board_free(board)
    if move:
      chess.move_free(move)
