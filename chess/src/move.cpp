#include <cstring>
#include <cassert>
#include <iostream>

#include "move.h"
#include "parseutils.h"

namespace game
{

using std::ostream;
using std::string;

Move::Move(const char *algNot) {
    // <frompc><frompos><...>
    frompc_ = pieceFromChar(algNot[0]);
    frompos_ = posFromStr(&algNot[1]);
    if (strchr(algNot, 'x')) {  // capture
        // std::cout << "MOVE " << algNot << " IS A CAPTURE" << std::endl;
        // <frompc><frompos><'x'><killpc><topos><'' | topc | 'e.p.'>
        switch (strlen(algNot)) {
            case 7:  // normal capture
                topc_ = frompc_;
                topos_ = posFromStr(&algNot[5]);
                killpc_ = pieceFromChar(algNot[4]);
                killpos_ = topos_;
                break;
            case 8:  // promotion capture
                topc_ = pieceFromChar(algNot[7]);
                topos_ = posFromStr(&algNot[5]);
                killpc_ = pieceFromChar(algNot[4]);
                killpos_ = topos_;
                break;
            case 11:  // en passant capture
                assert(strstr(algNot, "e.p."));
                topc_ = frompc_;
                topos_ = posFromStr(&algNot[5]);
                killpc_ = pieceFromChar(algNot[4]);
                killpos_ = (topos_ / 8 == 2) ? topos_ + 8 : topos_ - 8;
                break;
            default:
                std::cerr << "INVALID ALG NOT CASE" << std::endl;
                assert(false);
        }
    } else {  // not capture
        // std::cout << "MOVE " << algNot << " IS NOT A CAPTURE" << std::endl;
        // <frompc><frompos><topos><'' | topc>
        topc_ = (strlen(algNot) == 6) ? pieceFromChar(algNot[5]) : frompc_;
        topos_ = posFromStr(&algNot[3]);
        killpc_ = NOPC;
        killpos_ = NOPOS;
    }
}

Move::~Move(){};

Move &Move::operator=(const Move &other) {
    this->frompos_ = other.frompos_;
    this->topos_ = other.topos_;
    this->killpos_ = other.killpos_;
    this->frompc_ = other.frompc_;
    this->topc_ = other.topc_;
    this->killpc_ = other.killpc_;
    return *this;
}

bool Move::isCapture() const {
    return killpc_ != NOPC;
}

bool Move::isEnPassant() const {
    return isCapture() && (topos_ != killpos_);
}

bool Move::isPromotion() const {
    return frompc_ != topc_;
}

int Move::castleType() const {
    if (frompc_ == WKING) {
        if (frompos_ == POS('e', 1) && topos_ == POS('g', 1)) {
            return WKCASTLE;
        }
        if (frompos_ == POS('e', 1) && topos_ == POS('c', 1)) {
            return WQCASTLE;
        }
    }
    if (frompc_ == BKING) {
        if (frompos_ == POS('e', 8) && topos_ == POS('g', 8)) {
            return BKCASTLE;
        }
        if (frompos_ == POS('e', 8) && topos_ == POS('c', 8)) {
            return BQCASTLE;
        }
    }
    return 0;  // not castle move
}

bool Move::operator<(const Move &other) const {
        if (this->frompc_ != other.frompc_) {
            return this->frompc_ < other.frompc_;
        } else if (this->frompos_ != other.frompos_) {
            return this->frompos_ < other.frompos_;
        } else if (this->topc_ != other.topc_) {
            return this->topc_ < other.topc_;
        } else if (this->topos_ != other.topos_) {
            return this->topos_ < other.topos_;
        } else if (this->killpc_ != other.killpc_) {
            return this->killpc_ < other.killpc_;
        } else if (this->killpos_ != other.killpos_) {
            return this->killpos_ < other.killpos_;
        } else {
            return 0;  // equal
        }
}

string Move::algNot() const {
    // check if castling (special case)
    switch (this->castleType()) {
        case WKCASTLE:
        case BKCASTLE:
            return string("0-0");
        case WQCASTLE:
        case BQCASTLE:
            return string("0-0-0");
    }

    string ret("");
    // ret += pieceToChar(this->frompc_);
    ret += posToStr(this->frompos_);
    if (this->isCapture()) {
        ret += 'x';
        // ret += pieceToChar(this->killpc_);
    }
    ret += posToStr(this->topos_);
    if (this->isEnPassant()) ret += "e.p.";
    if (this->isPromotion()) ret += pieceToChar(this->topc_);
    return ret;
}

ostream &operator<<(ostream &os, const Move &move) {
    return os << move.algNot();
}

}  // namespace game
