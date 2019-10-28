%option noyywrap
%{
#include "algnot.h"
#include "parseutils.h"

using game::pos_t;
using game::pc_t;
using game::pieceFromChar;
using game::posFromStr;

static int MATCHED;  // 1 if matched last query, 0 otherwise
static pos_t FROMPOS, TOPOS, KILLPOS;
static pc_t FROMPC, TOPC, KILLPC;
%}

%%

P[a-h]7x[nbrqk][a-h]8[NBRQ]|p[a-h]2x[NBRQ][a-h]1[nbrq] {
    // promotion capture
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    KILLPC = pieceFromChar(yytext[4]);
    KILLPOS = posFromStr(&yytext[5]);
    TOPC = pieceFromChar(yytext[7]);
    TOPOS = KILLPOS;
    MATCHED = 1;
}

P[a-h]7[a-h]8[NBRQ]|p[a-h]2[a-h]1[nbrq] {
    // promotion
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    TOPOS = posFromStr(&yytext[3]);
    TOPC = pieceFromChar(yytext[5]);
    KILLPC = NOPC;
    KILLPOS = NOPOS;
    MATCHED = 1;
}

P[a-h]5xp[a-h]6"e.p." {
    // white ep capture
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    KILLPC = pieceFromChar(yytext[4]);
    TOPOS = posFromStr(&yytext[5]);
    KILLPOS = TOPOS - 8;
    TOPC = FROMPC;
    MATCHED = 1;
}

p[a-h]4xP[a-h]3"e.p." {
    // black ep capture
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    KILLPC = pieceFromChar(yytext[4]);
    TOPOS = posFromStr(&yytext[5]);
    KILLPOS = TOPOS + 8;
    TOPC = FROMPC;
    MATCHED = 1;
}

[PNBRQKpnbrqk][a-h][1-8][a-h][1-8] {
    // normal move
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    TOPOS = posFromStr(&yytext[3]);
    TOPC = FROMPC;
    KILLPC = NOPC;
    KILLPOS = NOPOS;
    MATCHED = 1;
}

[PNBRQK][a-h][1-8]x[pnbrq][a-h][1-8]|[pnbrqk][a-h][1-8]x[PNBRQ][a-h][1-8] {
    // normal capture
    FROMPC = pieceFromChar(yytext[0]);
    FROMPOS = posFromStr(&yytext[1]);
    KILLPC = pieceFromChar(yytext[4]);
    KILLPOS = posFromStr(&yytext[5]);
    TOPC = FROMPC;
    TOPOS = KILLPOS;
    MATCHED = 1;
}

%%

void clrRes() {
    MATCHED = 0;
    FROMPOS = 0;
    TOPOS = 0;
    KILLPOS = 0;
    FROMPC = 0;
    TOPC = 0;
    KILLPC = 0;
}

int game::parseAlgNot(const char *str, pos_t *fps, pos_t *tps, pos_t *kps, pc_t *fpc, pc_t *tpc, pc_t *kpc) {
    clrRes();
    YY_BUFFER_STATE bs = yy_scan_string(str);
    yylex();
    yy_delete_buffer(bs);

    // pack result
    *fps = FROMPOS;
    *tps = TOPOS;
    *kps = KILLPOS;
    *fpc = FROMPC;
    *tpc = TOPC;
    *kpc = KILLPC;
    return !MATCHED;
}
