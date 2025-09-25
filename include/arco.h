#ifndef ARCO_H
#define ARCO_H

#include "uris.h"
#include "sorted_set.h"

#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"
#include "lv2/urid/urid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

enum { ARCO_IN = 0, ARCO_OUT = 1 };

typedef struct {
	// Features
	LV2_URID_Map*  map;
	LV2_Log_Logger logger;

	// Ports
	const LV2_Atom_Sequence* in_port;
	LV2_Atom_Sequence* out_port;

	// URIs
	ArcoURIs uris;

	// Arp
	float arp_speed;
	int current_note, last_note_value;
	int time;
	float rate;
	SortedSet notes;
} Arco;

// Struct for a 3 byte MIDI event, used for writing notes
typedef struct {
    LV2_Atom_Event event;
    uint8_t msg[3];
} MIDINoteEvent;

#endif /* ARCO_H */