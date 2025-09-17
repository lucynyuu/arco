#include "notes.h"

uint64_t notes[WORDS];

void set_note(int k) {
    notes[k / WORD_BITS] |= (1ULL << (k % WORD_BITS));
}

void clear_note(int k) {
    notes[k / WORD_BITS] &= ~(1ULL << (k % WORD_BITS));
}

void clear_notes() {
    notes[0] = 0;
    notes[1] = 0;
}

int test_note(int k) {
    return (notes[k / WORD_BITS] >> (k % WORD_BITS)) & 1U;
}

/*
void iterate_notes() {
    for (int w = 0; w < WORDS; w++) {
        uint64_t word = notes[w];
        while (word) {
            int bit = __builtin_ctzll(word);
            int note = w * WORD_BITS + bit;
            printf("Note: %d\n", note);
            word &= word - 1;
        }
    }
}*/

// THIS BAD it MUST be fixed
int next_note_shit(int currentNote) {
    if (currentNote < 0 || currentNote >= MAX_NOTES) currentNote = -1;
    for (int i = 1; i <= MAX_NOTES; i++) {
        int candidate = (currentNote + i) % MAX_NOTES;
        if (notes[candidate / WORD_BITS] & (1ULL << (candidate % WORD_BITS)))
            return candidate;
    }
    return -1;
}

int sum_notes() {
    return notes[0]+notes[1];
}