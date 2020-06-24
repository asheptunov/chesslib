import sys, os
from ctypes import *

lib = CDLL(os.path.join(os.path.dirname(__file__), "..", "bin", "libchess.so"), RTLD_GLOBAL)

'''
STRUCTS
'''

class MOVE(Structure):
  _fields_ = [("frompos", c_ubyte),
              ("topos", c_ubyte),
              ("killpos", c_ubyte),
              ("frompc", c_ubyte),
              ("topc", c_ubyte),
              ("killpc", c_ubyte)]

class BOARD(Structure):
  _fields_ = [("ranks", c_uint*8),
              ("flags", c_uint)]

class ALST(Structure):
  _fields_ = [("len", c_size_t), ("cap", c_size_t), ("data", POINTER(c_void_p))]

'''
MOVE_T
'''

# move_t *move_make(pos_t frompos, pos_t topos, pos_t killpos, pc_t frompc, pc_t topc, pc_t killpc);
move_make = lib.move_make
move_make.argtypes = [c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte, c_ubyte]
move_make.restype = POINTER(MOVE)

# move_t *move_make_algnot(const char *algnot);
move_make_algnot_lib = lib.move_make_algnot
move_make_algnot_lib.argtypes = [c_char_p]
move_make_algnot_lib.restype = POINTER(MOVE)
def move_make_algnot(algnot):
  return move_make_algnot_lib(algnot.encode('ascii'))

# move_t *move_cpy(move_t *other);
move_cpy = lib.move_cpy
move_cpy.argtypes = [POINTER(MOVE)]
move_cpy.restype = POINTER(MOVE)

# void move_free(move_t *move);
move_free = lib.move_free
move_free.argtypes = [POINTER(MOVE)]

# int move_cmp(const move_t *a, const move_t *b);
move_cmp = lib.move_cmp
move_cmp.argtypes = [POINTER(MOVE), POINTER(MOVE)]
move_cmp.restype = c_int

# int move_is_cap(const move_t *move);
move_is_cap = lib.move_is_cap
move_is_cap.argtypes = [POINTER(MOVE)]
move_is_cap.restype = c_int

# int move_is_ep(const move_t *move);
move_is_ep = lib.move_is_ep
move_is_ep.argtypes = [POINTER(MOVE)]
move_is_ep.restype = c_int

# int move_is_promo(const move_t *move);
move_is_promo = lib.move_is_promo
move_is_promo.argtypes = [POINTER(MOVE)]
move_is_promo.restype = c_int

# int move_is_castle(const move_t *move);
move_is_castle = lib.move_is_castle
move_is_castle.argtypes = [POINTER(MOVE)]
move_is_castle.restype = c_int

# char *move_str(const move_t *move);
move_str_lib = lib.move_str
move_str_lib.argtypes = [POINTER(MOVE)]
move_str_lib.restype = c_char_p
def move_str(move):
  if not move:
    return None
  move_str_bytes = move_str_lib(move)
  move_str = move_str_bytes.decode('ascii')
  # free(move_str_bytes)
  return move_str

'''
BOARD_T
'''

# board_t *board_make(const char *fen);
board_make_lib = lib.board_make
board_make_lib.argtypes = [c_char_p]
board_make_lib.restype = POINTER(BOARD)
def board_make(fen):
  return board_make_lib(fen.encode('ascii'))

# board_t *board_copy(const board_t *other);
board_copy = lib.board_copy
board_copy.argtypes = [POINTER(BOARD)]
board_copy.restype = POINTER(BOARD)

# void board_free(const board_t *other);
board_free = lib.board_free
board_free.argtypes = [POINTER(BOARD)]

# void board_apply_move(board_t *board, const move_t *move);
board_apply_move = lib.board_apply_move
board_apply_move.argtypes = [POINTER(BOARD), POINTER(MOVE)]

# alst_t board_get_moves(const board_t *board);
board_get_moves = lib.board_get_moves
board_get_moves.argtypes = [POINTER(BOARD)]
board_get_moves.restype = POINTER(ALST)

# int board_is_mate(const board_t *board);
board_is_mate = lib.board_is_mate
board_is_mate.argtypes = [POINTER(BOARD)]
board_is_mate.restype = c_int

# int board_is_stalemate(const board_t *board);
board_is_stalemate = lib.board_is_stalemate
board_is_stalemate.argtypes = [POINTER(BOARD)]
board_is_stalemate.restype = c_int

# char *board_to_fen(const board_t *board);
board_to_fen_lib = lib.board_to_fen
board_to_fen_lib.argtypes = [POINTER(BOARD)]
board_to_fen_lib.restype = c_char_p
def board_to_fen(board):
  fen_bytes = board_to_fen_lib(board)
  fen = fen_bytes.decode('ascii')
  # free(fen_bytes)
  return fen

# char *board_to_tui(const board_t *board);
board_to_tui_lib = lib.board_to_tui
board_to_tui_lib.argtypes = [POINTER(BOARD)]
board_to_tui_lib.restype = c_char_p
def board_to_tui(board):
  tui_bytes = board_to_tui_lib(board)
  tui = tui_bytes.decode('ascii')
  # free(tui_bytes)
  return tui

# int _board_hit(const board_t *board, const int rk, const int offs, const int white);
_board_hit = lib._board_hit
_board_hit.argtypes = [POINTER(BOARD), c_int, c_int, c_int]
_board_hit.restype = c_int

'''
ALST_T
'''

# alst_t *alst_make(size_t cap);
alst_make = lib.alst_make
alst_make.argtypes = [c_size_t]
alst_make.restype = POINTER(ALST)

# void alst_free(alst_t *list);
alst_free = lib.alst_free
alst_free_func_type = CFUNCTYPE(None, c_void_p)
alst_free.argtypes = [POINTER(ALST), alst_free_func_type]

# void alst_put(alst_t *list, size_t i, void *val);
alst_put = lib.alst_put
alst_put.argtypes = [POINTER(ALST), c_size_t, c_void_p]

# void *alst_get(alst_t *list, size_t i);
alst_get = lib.alst_get
alst_get.argtypes = [POINTER(ALST), c_size_t]
alst_get.restype = c_void_p

# void alst_append(alst_t *list, void *val);
alst_append = lib.alst_append
alst_append.argtypes = [POINTER(ALST), c_void_p]
