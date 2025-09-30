#ifndef ARCO_CHORD_H
#define ARCO_CHORD_H

#include "arco.h"

typedef enum {
    ARCO_MINOR_CHORD = 3,
    ARCO_MAJOR_CHORD = 4
} ArcoChordType;

void arco_run_chord(Arco* self, uint32_t sample_count, ArcoChordType major);

#endif /* ARCO_CHORD_H */