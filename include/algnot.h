#pragma once

#include "defs.h"
#include "move.h"

/**
 * Parses a move from a string and returns extracted move properties. Returns with
 * a code 0 on success, nonzero on failure to parse.
 *
 * @param str the string to parse
 * @param fps frompos
 * @param tps topos
 * @param kps killpos
 * @param fpc frompc
 * @param tpc topc
 * @param kpc killpc
 * @return 0 on success; nonzero on failure.
 */
int algnot_parse(const char *str, move_t *move);
