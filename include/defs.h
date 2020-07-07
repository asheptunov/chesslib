#pragma once

#include <stdint.h>

// board positions (65 values; 7 bits)
typedef uint8_t pos_t;

// pieces (13 values; 4 bits)
typedef uint8_t pc_t;

// pieces
// defined in nice order for MVV-LVA
#define WPAWN 0
#define WKNIGHT 1
#define WBISHOP 2
#define WROOK 3
#define WQUEEN 4
#define WKING 5
#define BPAWN 6
#define BKNIGHT 7
#define BBISHOP 8
#define BROOK 9
#define BQUEEN 10
#define BKING 11
#define NOPC 12

// castling info
#define WKCASTLE 0b0001
#define WQCASTLE 0b0010
#define BKCASTLE 0b0100
#define BQCASTLE 0b1000

// which player
#define PLAYER 0b10000
#define WPLAYER 1
#define BPLAYER 0

// pos('a', 1) is 0, pos('b', 2) is 9, etc.
#define POS(offs, rank) ((((rank) - 1) << 3) + ((offs) - 'a'))
// same as POS but takes offs in 0-7 and rank in 0-7
#define POS2(offs, rank) (((rank) << 3) + (offs))
// also pos('a', 9)
#define NOPOS 64

// zeroes a position in the rank, takes offs in 0-7
#define ZEROPOS(offs, rank) ((rank) &= ~(0xf << ((offs) << 2)))

// same as ZEROPOS, but takes offs in 'a'-'h'
#define ZEROPOS2(offs, rank) ZEROPOS2(offs - 'a', rank)

// sets a position's piece in a rank to pc, takes offs in 0-7
#define SETPOS(offs, rank, pc) ((rank) |= ((pc) << ((offs) << 2)))

// same as SETPOS, but takes offs in 'a'-'h'
#define SETPOS2(offs, rank, pc) SETPOS2(offs - 'a', rank, pc)

// zeroes both positions, sets the old pc to nopc, new pc to topc
// takes offs in 0-7
#define MOVEPC(f_offs, t_offs, f_rank, t_rank, topc) \
    ZEROPOS((f_offs), (f_rank));                     \
    ZEROPOS((t_offs), (t_rank));                     \
    SETPOS((f_offs), (f_rank), NOPC);                \
    SETPOS((t_offs), (t_rank), (topc))

// same as MOVEPC, but takes offs in 'a'-'h'
#define MOVEPC2(f_offs, t_offs, f_rank, t_rank, topc) \
    MOVEPC((f_offs) - 'a', (t_offs) - 'a', (f_rank), (t_rank), (topc))

#define ZEROPLAYER(flags) ((flags) &= ~PLAYER)
#define SETPLAYER(player, flags) \
    ZEROPLAYER(flags);           \
    (flags) |= ((player) << 4)

#define ZEROCASTLE(flags) ((flags) &= ~0xf)
#define ZEROCASTLE2(flags, type) ((flags) &= ~(type))
#define SETCASTLE(castle, flags) ((flags) |= castle)

#define ZEROEP(flags) ((flags) &= ~(0xff << 8))
#define SETEP(ep_pos, flags) \
    ZEROEP(flags);           \
    ((flags) |= ((ep_pos) << 8))

#define ZEROWKING(flags) ((flags) &= ~(0xff << 16))
#define SETWKING(kingpos, flags) \
    ZEROWKING(flags);            \
    ((flags) |= ((kingpos) << 16))

#define ZEROBKING(flags) ((flags) &= ~(0xff << 24))
#define SETBKING(kingpos, flags) \
    ZEROBKING(flags);            \
    ((flags) |= ((kingpos) << 24))

#define FLAGS_WKCASTLE(flags) (!!((flags) & WKCASTLE))
#define FLAGS_WQCASTLE(flags) (!!((flags) & WQCASTLE))
#define FLAGS_BKCASTLE(flags) (!!((flags) & BKCASTLE))
#define FLAGS_BQCASTLE(flags) (!!((flags) & BQCASTLE))
#define FLAGS_CASTLE(flags) ((flags) & 0xf)

#define FLAGS_WPLAYER(flags) (!!((flags) & PLAYER))
#define FLAGS_BPLAYER(flags) (!((flags) & PLAYER))

#define FLAGS_EP(flags) (((flags) >> 8) & 0xff)

#define FLAGS_WKING(flags) (((flags) >> 16) & 0xff)
#define FLAGS_BKING(flags) (((flags) >> 24) & 0xff)

// checks if a piece is on the board, using rank in [0,7] and offs in [0,7]
#define ISPOS2(rk, offs) (((rk) >= 0 && (rk) < 8) && ((offs) >= 0 && (offs) < 8))

#define STARTING_BOARD "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"
