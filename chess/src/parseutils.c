#include <stdio.h>

#include "defs.h"
#include "arraylist.h"
#include "parseutils.h"

alst_t *_piece_char_map;
void _parse_utils_init();

void _parse_utils_init() {
    if (!_piece_char_map) {
      _piece_char_map = (alst_t *) malloc(sizeof(alst_t));
      if (!_piece_char_map) {
          fprintf(stderr, "init malloc error");
        exit(1);
      }
      *_piece_char_map = alst_make(12);
      alst_append(_piece_char_map, (void *) 'P');
      alst_append(_piece_char_map, (void *) 'N');
      alst_append(_piece_char_map, (void *) 'B');
      alst_append(_piece_char_map, (void *) 'R');
      alst_append(_piece_char_map, (void *) 'Q');
      alst_append(_piece_char_map, (void *) 'K');
      alst_append(_piece_char_map, (void *) 'p');
      alst_append(_piece_char_map, (void *) 'n');
      alst_append(_piece_char_map, (void *) 'b');
      alst_append(_piece_char_map, (void *) 'r');
      alst_append(_piece_char_map, (void *) 'q');
      alst_append(_piece_char_map, (void *) 'k');
    }
}

pos_t pos_from_str(const char *label) {
    if (*label == '-') {
        return NOPOS;
    }
    return POS(label[0], label[1] - '0');
}

// caller responsible for freeing returned buffer
char *pos_to_str(const pos_t pos) {
    if (pos == NOPOS) {
        return "-";
    }
    char *ret = (char *) malloc(3 * sizeof(char));
    if (!ret) {
        fprintf(stderr, "pos_to_str malloc error\n");
        exit(1);
    }
    ret[0] = (pos % 8) + 'a';
    ret[1] = (pos / 8) + '1';
    ret[2] = '\0';
    return ret;
}

pc_t piece_from_char(const char label) {
    _parse_utils_init();
    size_t ret = alst_index_of(_piece_char_map, (void *) label);
    return (ret == -1) ? NOPC : ((pc_t) ret);
}

// caller responsible for freeing returned buffer
char *piece_to_str(const pc_t piece) {
    _parse_utils_init();
    char *ret = (char *) malloc(2 * sizeof(char));
    ret[0] = (piece >= WPAWN && piece <= BKING) ? (char) alst_get(_piece_char_map, (size_t) piece) : '-';
    ret[1] = '\0';
    return ret;
}
