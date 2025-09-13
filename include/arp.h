#ifndef ARCO_ARP_H
#define ARCO_ARP_H

#include "arco.h"

static const int arp_intervals[] = {0, 4, 7};
static const int arp_steps = 3;
static const int arp_spacing = 2500; // frames between notes (adjust for tempo)

void arco_run_arp(Arco* self, uint32_t sample_count);

#endif /* ARCO_ARP_H */