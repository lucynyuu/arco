#ifndef ARCO_NOTES_H
#define ARCO_NOTES_H
/**
 * Hey I love this solution I wanted to avoid using a sorted set as it seemed like to much overhead
 * I figured I would bring my love of bit manipulation into this project too xD
 */
#include "arco.h"

#define MAX_NOTES 128
#define WORD_BITS 64
#define WORDS ((MAX_NOTES + WORD_BITS - 1) / WORD_BITS)

void set_note(int k);

void clear_note(int k);

void clear_notes();

int test_note(int k);

// void iterate_notes();

int next_note_shit(int currentNote);

int sum_notes();

#endif /* ARCO_NOTES_H */ 