from ctypes import *

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


ctr = 0  # for debugging

def minimax(evaluate, board, depth):
  '''
  performs minimax search from the current board position, exploring down to a given depth;
  returns the optimal move from the current position (None if there are no moves, or depth is 0) and the value of the board when applying that move
  '''
  global ctr
  retpaths = []

  if depth <= 0:
    ctr += 1
    return (None, evaluate(board), retpaths)

  # get moves
  moves_alst = chess.board_get_moves(board)
  moves = chess.alst_to_vector(moves_alst, POINTER(chess.MOVE))
  chess.alst_free(moves_alst)

  # print(len(moves), [chess.move_str(m) for m in moves])

  if len(moves) == 0:
    # active player has no moves; loss or stalemate
    ctr += 1
    return (None, -100_000 - depth, retpaths)

  max_utility = -float('inf')
  max_utility_move = None
  for move in moves:
    future_board = chess.board_copy(board)
    chess.board_apply_move(future_board, move)

    opp_utility, _, childpaths = minimax(evaluate, future_board, depth - 1)

    if len(childpaths) == 0:
      retpaths.append(chess.move_str(move))
    else:
      for i in range(len(childpaths)):
        retpaths.append(chess.move_str(move) + '>' + childpaths[i])

    utility = -opp_utility  # negate since this is the opponent's maximal utility, which is our minimal utility
    if (utility > max_utility):
      max_utility = utility
      max_utility_move = move

  return (max_utility_move, max_utility, retpaths)

# use for debugging
if __name__ == '__main__':
    board = chess.board_make('rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -')
    utility, move, paths = minimax.minimax(minimax.evaluate, board, 3)
    print('board', os.linesep, chess.board_to_tui(board))
    print('paths', os.linesep, os.linesep.join(sorted(paths)))
    print('nodes explored', minimax.ctr)
