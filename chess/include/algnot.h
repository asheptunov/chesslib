#pragma once

#include "defs.h"

namespace game {

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
int parseAlgNot(const char *str, pos_t *fps, pos_t *tps, pos_t *kps, pc_t *fpc, pc_t *tpc, pc_t *kpc);

}  // namespace game
