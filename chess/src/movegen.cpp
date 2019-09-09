#include <cstdlib>
#include <vector>
#include <map>

#include "board.h"
#include "move.h"

using std::vector;
using std::map;

namespace game
{

#define UP (8)
#define RT (1)
#define DN (-8)
#define LT (-1)

#define UL (7)
#define UR (9)
#define DR (-7)
#define DL (-9)

#define DIAG \
    {UP, RT}, {UP * 2, RT * 2}, {UP * 3, RT * 3}, {UP * 4, RT * 4}, {UP * 5, RT * 5}, {UP * 6, RT * 6}, {UP * 7, RT * 7}, \
    {UP, LT}, {UP * 2, LT * 2}, {UP * 3, LT * 3}, {UP * 4, LT * 4}, {UP * 5, LT * 5}, {UP * 6, LT * 6}, {UP * 7, LT * 7}, \
    {DN, RT}, {DN * 2, RT * 2}, {DN * 3, RT * 3}, {DN * 4, RT * 4}, {DN * 5, RT * 5}, {DN * 6, RT * 6}, {DN * 7, RT * 7}, \
    {DN, LT}, {DN * 2, LT * 2}, {DN * 3, LT * 3}, {DN * 4, LT * 4}, {DN * 5, LT * 5}, {DN * 6, LT * 6}, {DN * 7, LT * 7}  \

#define LATERAL \
    {UP, 0}, {UP * 2, 0}, {UP * 3, 0}, {UP * 4, 0}, {UP * 5, 0}, {UP * 6, 0}, {UP * 7, 0}, \
    {DN, 0}, {DN * 2, 0}, {DN * 3, 0}, {DN * 4, 0}, {DN * 5, 0}, {DN * 6, 0}, {DN * 7, 0}, \
    {0, RT}, {0, 2 * RT}, {0, 3 * RT}, {0, 4 * RT}, {0, 5 * RT}, {0, 6 * RT}, {0, 7 * RT}, \
    {0, LT}, {0, 2 * LT}, {0, 3 * LT}, {0, 4 * LT}, {0, 5 * LT}, {0, 6 * LT}, {0, 7 * LT}  \

static map<int, vector<vector<int8_t>>> moves =
   {{WKNIGHT, {{2 * UP, RT}, {2 * UP, LT}, {2 * DN, RT}, {2 * DN, LT}, {UP, 2 * RT}, {UP, 2 * LT}, {DN, 2 * RT}, {DN, 2 * LT}}},
    {WBISHOP, {DIAG}},
    {WROOK,   {LATERAL}},
    {WQUEEN,  {DIAG, LATERAL}},
    {WKING,   {{UP, 0}, {DN, 0}, {0, RT}, {0, LT}, {UP, RT}, {UP, LT}, {DN, RT}, {DN, LT}}}};

vector<Move> Board::generateMoves() const {
    vector<Move> ret;
    // TODO:
    return ret;
}

} // namespace game
