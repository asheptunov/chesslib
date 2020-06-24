#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "arraylist.h"
#include "parseutils.h"

const char *_piece_str_map[12] = {"P", "N", "B", "R", "Q", "K", "p", "n", "b", "r", "q", "k"};

pos_t pos_from_str(const char *label) {
    if (*label == '-') {
        return NOPOS;
    }
    return POS(label[0], label[1] - '0');
}

// caller responsible for freeing returned buffer
char *pos_to_str(const pos_t pos) {
    char *ret;
    if (pos == NOPOS) {
        ret = (char *) malloc(2 * sizeof(char));
        if (!ret) {
            fprintf(stderr, "malloc error in pos_to_str (NOPOS)\n");
            exit(1);
        }
        memcpy(ret, "-", 2);
        return ret;
    }
    ret = (char *) malloc(3 * sizeof(char));
    if (!ret) {
        fprintf(stderr, "malloc error in pos_to_str (not NOPOS)\n");
        exit(1);
    }
    ret[0] = (pos % 8) + 'a';
    ret[1] = (pos / 8) + '1';
    ret[2] = '\0';
    return ret;
}

pc_t piece_from_char(const char label) {
    // find index of label in the list
    for (int i = 0; i < 12; ++i) {
        if (*(_piece_str_map[i]) == label) {
            return (pc_t) i;
        }
    }
    return NOPC;
}

const char *piece_to_str(const pc_t piece) {
    if (piece <= BKING) {
        return _piece_str_map[piece];
    } else {
        return "-";
    }
}
