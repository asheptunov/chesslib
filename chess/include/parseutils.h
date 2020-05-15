#ifndef PARSEUTILS_H
#define PARSEUTILS_H

#include "defs.h"
#include "arraylist.h"

alst_t *piece_char_map;
void init();

pos_t pos_from_str(const char *label);
char *pos_to_str(const pos_t pos);
pc_t piece_from_char(const char label);
char *piece_to_str(const pc_t piece);

#endif  // PARSEUTILS_H
