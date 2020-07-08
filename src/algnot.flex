%option noyywrap
%option nounput
%option noinput
%{
#include "defs.h"
#include "algnot.h"
#include "parseutils.h"

static int MATCHED;  // 1 if matched last query, 0 otherwise
static pos_t FROMPOS, TOPOS, KILLPOS;
static pc_t FROMPC, TOPC, KILLPC;
%}

%%

P[a-h]7x[nbrqk][a-h]8[NBRQ]|p[a-h]2x[NBRQ][a-h]1[nbrq] {
    // promotion capture
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    KILLPC = piece_from_char(yytext[4]);
    KILLPOS = pos_from_str(&yytext[5]);
    TOPC = piece_from_char(yytext[7]);
    TOPOS = KILLPOS;
    MATCHED = 1;
}

P[a-h]7[a-h]8[NBRQ]|p[a-h]2[a-h]1[nbrq] {
    // promotion
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    TOPOS = pos_from_str(&yytext[3]);
    TOPC = piece_from_char(yytext[5]);
    KILLPC = NOPC;
    KILLPOS = NOPOS;
    MATCHED = 1;
}

P[a-h]5xp[a-h]6"e.p." {
    // white ep capture
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    KILLPC = piece_from_char(yytext[4]);
    TOPOS = pos_from_str(&yytext[5]);
    KILLPOS = TOPOS - 8;
    TOPC = FROMPC;
    MATCHED = 1;
}

p[a-h]4xP[a-h]3"e.p." {
    // black ep capture
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    KILLPC = piece_from_char(yytext[4]);
    TOPOS = pos_from_str(&yytext[5]);
    KILLPOS = TOPOS + 8;
    TOPC = FROMPC;
    MATCHED = 1;
}

[PNBRQKpnbrqk][a-h][1-8][a-h][1-8] {
    // normal move
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    TOPOS = pos_from_str(&yytext[3]);
    TOPC = FROMPC;
    KILLPC = NOPC;
    KILLPOS = NOPOS;
    MATCHED = 1;
}

[PNBRQK][a-h][1-8]x[pnbrq][a-h][1-8]|[pnbrqk][a-h][1-8]x[PNBRQ][a-h][1-8] {
    // normal capture
    FROMPC = piece_from_char(yytext[0]);
    FROMPOS = pos_from_str(&yytext[1]);
    KILLPC = piece_from_char(yytext[4]);
    KILLPOS = pos_from_str(&yytext[5]);
    TOPC = FROMPC;
    TOPOS = KILLPOS;
    MATCHED = 1;
}

%%

static void clrRes() {
    MATCHED = 0;
    FROMPOS = 0;
    TOPOS = 0;
    KILLPOS = 0;
    FROMPC = 0;
    TOPC = 0;
    KILLPC = 0;
}

#ifdef CHESSLIB_QWORD_MOVE
int algnot_parse(const char *str, move_t *move) {
    clrRes();
    YY_BUFFER_STATE bs = yy_scan_string(str);  // parse
    yylex();
    yy_delete_buffer(bs);  // cleanup
    *move = MVMAKE(FROMPOS, TOPOS, KILLPOS, FROMPC, TOPC, KILLPC);
    return !MATCHED;
}
#else
int algnot_parse(const char *str, move_t *move) {
    clrRes();
    YY_BUFFER_STATE bs = yy_scan_string(str);  // parse
    yylex();
    yy_delete_buffer(bs);  // cleanup
    move->frompos = FROMPOS;  // pack result
    move->topos = TOPOS;
    move->killpos = KILLPOS;
    move->frompc = FROMPC;
    move->topc = TOPC;
    move->killpc = KILLPC;
    return !MATCHED;
}
#endif
