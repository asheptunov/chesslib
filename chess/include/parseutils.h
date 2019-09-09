#pragma once

#include <string>

#include "defs.h"

namespace game {

using std::string;

pos_t posFromStr(const char *label);
string posToStr(const pos_t pos);
pc_t pieceFromChar(const char label);
char pieceToChar(const pc_t piece);

}  // namespace game
