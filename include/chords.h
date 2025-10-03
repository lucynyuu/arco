#ifndef CHORDS_H
#define CHORDS_H

typedef struct {
    unsigned char notes[3];
    unsigned char size;
} Chord;

static void init_chord(Chord* chord, unsigned char size, unsigned char n1, unsigned char n2, unsigned char n3) {
    chord->size = size;
    chord->notes[0] = n1;
    chord->notes[1] = n2;
    chord->notes[2] = n3;
}

#endif /* CHORDS_H */