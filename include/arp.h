#ifndef ARCO_ARP_H
#define ARCO_ARP_H

#include "arco.h"

typedef struct {
    LV2_Atom_Event event;
    uint8_t msg[3];
} MIDINoteEvent;

// Simple arpeggio pattern: +0, +4, +7 semitones
static const int arp_intervals[] = {0, 4, 7};
static const int arp_steps = 3;
static const int arp_spacing = 2400; // frames between notes (adjust for tempo)

void arco_run_arp(Arco* self, uint32_t sample_count);

#endif /* ARCO_ARP_H */