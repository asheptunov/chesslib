#include <string.h>
#include <iostream>
#include <cstdlib>

#include "board.h"
#include "parseutils.h"

namespace game
{

using std::cout;
using std::endl;

Board::Board(const char *fen) {
    char *fen_ = strdup(fen);
    char *board_p = strtok(fen_, " ");
    char *player_p = strtok(NULL, " ");
    char *castling_p = strtok(NULL, " ");
    char *ep_p = strtok(NULL, " ");
    const char *fenranks[8];
    fenranks[0] = strtok(board_p, "/");
    for (int i = 1; i < 8; ++i) {
        fenranks[i] = strtok(NULL, "/");
    }

    // SETUP BOARD
    int offs, pc;
    for (int rk = 8; rk > 0; --rk) {
        ranks_[rk - 1] = 0xcccccccc;  // init to NOPC
        offs = 0;
        for (unsigned int i = 0; i < strlen(fenranks[8 - rk]); ++i) {
            pc = pieceFromChar(fenranks[8 - rk][i]);
            ZEROPOS(offs, ranks_[rk - 1]);
            SETPOS(offs, ranks_[rk - 1], pc);
            offs += (pc == NOPC) ? fenranks[8 - rk][i] - '0' : 1;
        }
    }
    free(fen_);

    // SETUP PLAYER
    SETPLAYER((strcmp("w", player_p) ? BPLAYER : WPLAYER), flags_);

    // SETUP CASTLING
    ZEROCASTLE(flags_);
    if (strchr(castling_p, 'K')) SETCASTLE(WKCASTLE, flags_);
    if (strchr(castling_p, 'Q')) SETCASTLE(WQCASTLE, flags_);
    if (strchr(castling_p, 'k')) SETCASTLE(BKCASTLE, flags_);
    if (strchr(castling_p, 'q')) SETCASTLE(BQCASTLE, flags_);

    // SET EN PASSANT
    SETEP(strchr(ep_p, '-') ? NOPOS : POS(ep_p[0], ep_p[1] - '0'), flags_);
}

Board::Board(const Board &other) {
    memcpy(&this->ranks_, &other.ranks_, sizeof(int32_t) << 3);
    this->flags_ = other.flags_;
}

Board::~Board(){};

Board &Board::operator=(const Board &other) {
    memcpy(&this->ranks_, &other.ranks_, sizeof(ranks_));
    this->flags_ = other.flags_;
    return *this;
}

void Board::applyMove(const Move &move) {
    // KILL VICTIM
    if (move.isCapture()) {
        int k_rk = move.killpos_ >> 3;
        int k_offs = move.killpos_ & 7;
        ZEROPOS(k_offs, ranks_[k_rk]);
        SETPOS(k_offs, ranks_[k_rk], NOPC);
    }

    // MOVE PIECE
    int f_rk = move.frompos_ >> 3;  // pos / 8
    int t_rk = move.topos_ >> 3;
    int f_offs = move.frompos_ & 7;  // pos % 8
    int t_offs = move.topos_ & 7;
    MOVEPC(f_offs, t_offs, ranks_[f_rk], ranks_[t_rk], move.topc_);

    // MOVE ROOK IF CASTLE
    switch (move.castleType()) {
        case 0: break;
        case WKCASTLE:
            MOVEPC2('h', 'f', ranks_[0], ranks_[0], WROOK); break;
        case WQCASTLE:
            MOVEPC2('a', 'd', ranks_[0], ranks_[0], WROOK); break;
        case BKCASTLE:
            MOVEPC2('h', 'f', ranks_[7], ranks_[7], BROOK); break;
        case BQCASTLE:
            MOVEPC2('a', 'd', ranks_[7], ranks_[7], BROOK); break;
    }

    // UPDATE CASTLING RIGHTS
    switch (move.frompc_) {
        case WROOK:
            ZEROCASTLE2(flags_, move.frompos_ == POS('a', 1) ? WQCASTLE : WKCASTLE);
            break;
        case WKING:
            ZEROCASTLE2(flags_, WKCASTLE);
            ZEROCASTLE2(flags_, WQCASTLE);
            break;
        case BROOK:
            ZEROCASTLE2(flags_, move.frompos_ == POS('a', 8) ? BQCASTLE : BKCASTLE);
            break;
        case BKING:
            ZEROCASTLE2(flags_, BKCASTLE);
            ZEROCASTLE2(flags_, BQCASTLE);
            break;
    }

    // UPDATE EN PASSANT RIGHTS
    SETEP(NOPOS, flags_);
    if ((move.frompc_ == WPAWN || move.frompc_ == BPAWN) \
        && (abs(move.topos_ - move.frompos_) == 16)) {
        SETEP((move.frompos_ + move.topos_) >> 1, flags_);
    }

    // FLIP PLAYERS
    int player = FLAGS_WPLAYER(flags_) ? BPLAYER : WPLAYER;
    SETPLAYER(player, flags_);

    // UPDATE SIGNATURE
    // TODO:
}

string Board::toFen() const {
    string ret;
    // BOARD
    int blanks;
    uint32_t rank;
    for (int rk = 8; rk > 0; --rk) {
        // for finding sequences of x's (unoccupied)
        blanks = 0;
        rank = this->ranks_[rk - 1];
        for (int offs = 'a'; offs <= 'h'; ++offs) {
            int pc = rank & 0xf;  // one byte / pc at a time
            if (pc == NOPC) {
                ++blanks;
            } else {
                if (blanks) {
                    ret.append(std::to_string(blanks));
                    blanks = 0;
                }
                ret.push_back(pieceToChar(pc));
            }
            rank >>= 4;
        }
        // flush any trailing blank sequences
        if (blanks) ret.append(std::to_string(blanks));
        if (rk > 1) ret.push_back('/');
    }
    ret.push_back(' ');
    // PLAYER
    ret.push_back(FLAGS_WPLAYER(this->flags_) ? 'w' : 'b');
    ret.push_back(' ');
    // CASTLING
    if (FLAGS_CASTLE(this->flags_)) {
        if (flags_ & WKCASTLE) ret.push_back('K');
        if (flags_ & WQCASTLE) ret.push_back('Q');
        if (flags_ & BKCASTLE) ret.push_back('k');
        if (flags_ & BQCASTLE) ret.push_back('q');
    } else {
        ret.push_back('-');
    }
    ret.push_back(' ');
    // EN PASSANT
    ret.append(posToStr(FLAGS_EP(this->flags_)));  // NOPOS auto handled
    return ret;
}

ostream &operator<<(ostream &os, const Board &board) {
    os << "    a b c d e f g h" << endl << endl;
    uint32_t rank;
    for (int rk = 8; rk > 0; --rk) {
        os << std::to_string(rk) << "  ";
        rank = board.ranks_[rk - 1];
        for (int offs = 'a'; offs <= 'h'; ++offs) {
            os << ' ' << pieceToChar(rank & 0xf);
            rank >>= 4;
        }
        os << "   " << std::to_string(rk) << endl;
    }
    os << endl << "    a b c d e f g h";
    return os;
}

}  // namespace game
