#pragma once

#include "defs.h"
#include "arraylist.h"

pos_t pos_from_str(const char *label);

char *pos_to_str(const pos_t pos);

pc_t piece_from_char(const char label);

const char *piece_to_str(const pc_t piece);
