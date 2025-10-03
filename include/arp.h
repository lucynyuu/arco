#ifndef ARCO_ARP_H
#define ARCO_ARP_H

#include "arco.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void arco_run_arp(Arco* self, uint32_t sample_count, ArcoChordType chord_type, int pattern_type);

#endif /* ARCO_ARP_H */