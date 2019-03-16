#include <cstring>

#include "parseutils.h"

namespace game {

pos_t posFromStr(const char *label) {
    if (*label == '-') return NOPOS;
    return POS(label[0], label[1] - '0');
}

string posToStr(const pos_t pos) {
    if (pos == NOPOS) return string("-");
    return string({static_cast<char>((pos & 7) + 'a'), static_cast<char>((pos >> 3) + '1')});
}

pc_t pieceFromChar(const char label) {
    return pieceFromCharMap.count(label) ? pieceFromCharMap[label] : NOPC;
}

char pieceToChar(const pc_t piece) {
    return pieceToCharMap.count(piece) ? pieceToCharMap[piece] : '-';
}

}  // namespace game
