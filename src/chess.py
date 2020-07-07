import sys, os
import configparser
import re
from ctypes import *

'''
load library from config (searches for 'pychess.ini' in this file's root directory)
'''

config = configparser.ConfigParser()
config.read(os.path.join(os.path.dirname(__file__), 'pychess.ini'))  # ./pychess.ini
lib = WinDLL(config['libchess']['nt_path'], RTLD_GLOBAL) if os.name == 'nt' else CDLL(config['libchess']['posix_path'], RTLD_GLOBAL)

'''
UTIL
'''

STARTING_FEN = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -'
_FEN_RK_1_8 = r'[rnbqkRNBQK1-8]+'
_FEN_RK_2_7 = r'[rnbqkpRNBQKP1-8]+'
_FEN_PLAYER = r'[wb]'
_FEN_CASTLING = r'(-|K(Q|)(k|)(q|)|(K|)Q(k|)(q|)|(K|)(Q|)k(q|)|(K|)(Q|)(k|)q)'
_FEN_EP = r'(-|[a-h][36])'
_FEN_RGX = ' '.join([r'\/'.join([_FEN_RK_1_8, *(6*[_FEN_RK_2_7]), _FEN_RK_1_8]), _FEN_PLAYER, _FEN_CASTLING, _FEN_EP])

def _good_fen(fen):
  return re.search(_FEN_RGX, fen) is not None

'''
STRUCT DEFINITIONS
'''

class BOARD(Structure):
  _fields_ = [("ranks", c_uint*8),
              ("flags", c_uint)]

BOARD_T = BOARD
BOARD_PTR_T = POINTER(BOARD)

class MOVE(Structure):
  _fields_ = [("frompos", c_ubyte),
              ("topos", c_ubyte),
              ("killpos", c_ubyte),
              ("frompc", c_ubyte),
              ("topc", c_ubyte),
              ("killpc", c_ubyte)]

MOVE_T = MOVE
MOVE_PTR_T = POINTER(MOVE_T)

class ALST(Structure):
  _fields_ = [("len", c_size_t), ("cap", c_size_t), ("data", POINTER(c_void_p))]

ALST_T = ALST
ALST_PTR_T = POINTER(ALST_T)

'''
PYTHON CLASS WRAPPERS
'''

'''
MOVE
'''

move_make_lib = lib.move_make
move_make_lib.argtypes = [c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
move_make_lib.restype = POINTER(MOVE)

move_make_algnot_lib = lib.move_make_algnot
move_make_algnot_lib.argtypes = [c_char_p]
move_make_algnot_lib.restype = POINTER(MOVE)

move_cpy_lib = lib.move_cpy
move_cpy_lib.argtypes = [POINTER(MOVE)]
move_cpy_lib.restype = POINTER(MOVE)

move_free_lib = lib.move_free
move_free_lib.argtypes = [POINTER(MOVE)]

move_cmp_lib = lib.move_cmp
move_cmp_lib.argtypes = [POINTER(MOVE), POINTER(MOVE)]
move_cmp_lib.restype = c_int

move_is_cap_lib = lib.move_is_cap
move_is_cap_lib.argtypes = [POINTER(MOVE)]
move_is_cap_lib.restype = c_int

move_is_ep_lib = lib.move_is_ep
move_is_ep_lib.argtypes = [POINTER(MOVE)]
move_is_ep_lib.restype = c_int

move_is_promo_lib = lib.move_is_promo
move_is_promo_lib.argtypes = [POINTER(MOVE)]
move_is_promo_lib.restype = c_int

move_is_castle_lib = lib.move_is_castle
move_is_castle_lib.argtypes = [POINTER(MOVE)]
move_is_castle_lib.restype = c_int

move_str_lib = lib.move_str
move_str_lib.argtypes = [POINTER(MOVE)]
move_str_lib.restype = c_char_p

class Move:
  def __init__(self, move):
    if isinstance(move, Move):
      self._move = move._move
    elif isinstance(move, MOVE_PTR_T):
      self._move = move
    elif isinstance(move, MOVE_T):
      self._move = pointer(move)
    else:
      raise TypeError('not a move %s' % move)

  def __cmp__(self, other):
    if isinstance(other, Move):
      return move_cmp_lib(self._move, other._move)
    elif isinstance(other, MOVE_PTR_TYPE):
      return move_cmp_lib(self._move, other)
    elif isinstance(other, MOVE_T):
      return move_cmp_lib(self._move, pointer(other))
    else:
      raise TypeError('not a move %s' % other)

  def __str__(self):
    return move_str_lib(self._move).decode('ascii')

  def __del__(self):
    move_free_lib(self._move)

  @classmethod
  def from_values(cls, frompos, topos, killpos, frompc, topc, killpc):
    '''
    Makes a move from position / piece arguments.
    Positions are enumerated from 0 to 63 in row (rank) major order, i.e., A1,...H1,B2,...,H8.
    Nonexistent positions are marked by position number 64.
    White pieces are enumerated from 0 to 5 as follows: pawn, knight, bishop, rook, queen, king.
    Black pieces are enumerated from 6 to 11 as follows: pawn,..., king.
    Empty squares are marked by piece number 12.
    '''
    return cls(move_make_lib(frompos, topos, killpos, fropc, topc, killpc))

  @classmethod
  def from_algnot(cls, algnot):
    '''
    Returns a move made from algebraic notation.
    '''
    return cls(move_make_algnot_lib(algnot.encode('ascii')))

  @classmethod
  def from_move(cls, move):
    '''
    Returns a deep copy of the move.
    '''
    if isinstance(move, Move):
      return cls(move_cpy_lib(move._move))
    elif isinstance(move, MOVE_PTR_T):
      return cls(move_cpy_lib(move))
    elif isinstance(move, MOVE_T):
      return cls(move_cpy_lib(pointer(move)))
    else:
      raise TypeError('not a move %s' % move)

  def is_cap(self):
    '''
    Returns true iff the move is a capture.
    '''
    return move_is_cap_lib(self._move)

  def is_ep(self):
    '''
    Returns true iff the move is an en passant capture.
    '''
    return move_is_ep_lib(self._move)

  def is_promo(self):
    '''
    Returns true iff the move is a promotion.
    '''
    return move_is_promo_lib(self._move)

  def is_castle(self):
    '''
    Returns true iff the move is a castle.
    '''
    return move_is_castle_lib(self._move)

  def move(self):
    '''
    Returns the underlying move_t* pointer.
    '''
    return self._move

'''
BOARD
'''

board_make_lib = lib.board_make
board_make_lib.argtypes = [c_char_p]
board_make_lib.restype = POINTER(BOARD)

board_copy_lib = lib.board_copy
board_copy_lib.argtypes = [POINTER(BOARD)]
board_copy_lib.restype = POINTER(BOARD)

board_free_lib = lib.board_free
board_free_lib.argtypes = [POINTER(BOARD)]

board_apply_move_lib = lib.board_apply_move
board_apply_move_lib.argtypes = [POINTER(BOARD), POINTER(MOVE)]

board_get_moves_lib = lib.board_get_moves
board_get_moves_lib.argtypes = [POINTER(BOARD)]
board_get_moves_lib.restype = POINTER(ALST)

board_is_mate_lib = lib.board_is_mate
board_is_mate_lib.argtypes = [POINTER(BOARD)]
board_is_mate_lib.restype = c_int

board_is_stalemate_lib = lib.board_is_stalemate
board_is_stalemate_lib.argtypes = [POINTER(BOARD)]
board_is_stalemate_lib.restype = c_int

board_to_fen_lib = lib.board_to_fen
board_to_fen_lib.argtypes = [POINTER(BOARD)]
board_to_fen_lib.restype = c_char_p

board_to_tui_lib = lib.board_to_tui
board_to_tui_lib.argtypes = [POINTER(BOARD)]
board_to_tui_lib.restype = c_char_p

_board_hit_lib = lib._board_hit
_board_hit_lib.argtypes = [POINTER(BOARD), c_int, c_int, c_int]
_board_hit_lib.restype = c_int

class Board:
  def __init__(self, board):
    if isinstance(board, Board):
      self._board = board._board
    elif isinstance(board, BOARD_PTR_T):
      self._board = board
    elif isinstance(board, BOARD_T):
      self._board = pointer(board)
    else:
      raise TypeError('not a board %s' % board)

  def __str__(self):
    return self.to_fen()

  def __del__(self):
    board_free_lib(self._board)

  @classmethod
  def from_fen(cls, fen, chk=True):
    '''
    Returns a board made from a FEN.
    Verifies the FEN is valid if specified (validation may produce false positives).
    '''
    if chk and not _good_fen(fen):
      raise ValueError('bad fen %s' % fen)
    return cls(board_make_lib(fen.encode('ascii')))

  @classmethod
  def from_board(cls, board):
    '''
    Returns a deep copy of the board.
    '''
    if isinstance(board, Board):
      return cls(board_copy_lib(board._board))
    elif isinstance(board, BOARD_PTR_T):
      return cls(board_copy_lib(board))
    elif isinstance(board, BOARD_T):
      return cls(board_copy_lib(pointer(board)))
    else:
      raise TypeError('not a board %s' % board)

  def apply_move(self, move, in_place=False):
    '''
    Applies the move to the board, and returns the resulting board.
    Returns a copy with the move applied, unless in_place is specified.
    '''
    future_board = self if in_place else Board.from_board(self._board)
    if isinstance(move, Move):
      board_apply_move_lib(future_board._board, move._move)
    elif isinstance(move, MOVE_PTR_T):
      board_apply_move_lib(future_board._board, move)
    elif isinstance(move, MOVE_T):
      board_apply_move_lib(future_board._board, pointer(move))
    else:
      raise TypeError('not a move %s' % move)
    return future_board

  def get_moves(self):
    '''
    Returns a list of all the legal moves from this board position.
    '''
    # don't expose alst_t for simplicity
    # convert to iterable instead
    alst = board_get_moves_lib(self._board)
    moves = [Move.from_move(cast(alst_get_lib(alst, i), MOVE_PTR_T)) for i in range(alst.contents.len)]
    alst_free_lib(alst, alst_move_free_func)
    return moves

  def is_mate(self):
    '''
    Returns True iff the current player is under checkmate.
    '''
    return board_is_mate_lib(self._board)

  def is_stalemate(self):
    '''
    Returns true iff the game is under stalemate, or if there is a draw by insufficient material.
    Does not account for draws by repitition, and for highly niche draws by insufficient material.
    '''
    return board_is_stalemate_lib(self._board)

  def to_fen(self):
    '''
    Returns the FEN representation of the board, excluding the halfmove clock and fullmove number.
    '''
    return board_to_fen_lib(self._board).decode('ascii')

  def to_tui(self):
    '''
    Returns a TUI representation of the board
    '''
    return board_to_tui_lib(self._board).decode('ascii')

  def _hit(self, rk, fl, by_blk):
    '''
    Returns true if the 0-indexed position (rk [corresponding to ranks 1-8], fl [corresponding to files A-H])
    is hit by the specified player (by_blk corresponding to hits by black).
    '''
    return _board_hit_lib(self._board, rk, fl, by_blk)

  def board(self):
    '''
    Returns the underlying board_t* pointer.
    '''
    return self._board

'''
ALST
'''

alst_free_lib = lib.alst_free
alst_free_func_type = CFUNCTYPE(None, c_void_p)
alst_move_free_func = cast(move_free_lib, alst_free_func_type)
alst_free_lib.argtypes = [POINTER(ALST), alst_free_func_type]

alst_get_lib = lib.alst_get
alst_get_lib.argtypes = [POINTER(ALST), c_size_t]
alst_get_lib.restype = c_void_p
