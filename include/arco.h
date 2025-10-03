#ifndef ARCO_H
#define ARCO_H

#include "uris.h"
#include "sorted_set.h"
#include "chords.h"

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

enum { 	
	ARCO_IN = 0, 
	ARCO_OUT = 1, 
	ARCO_CONTROL = 2, 
	ARCO_REVERSE = 3, 
	ARCO_ARP_ENABLE = 4, 
	ARCO_CHORD_ENABLE = 5, 
	ARCO_CHORD = 6, 
	ARCO_OCTAVE = 7,
	ARCO_ARP_PATTERN = 8
};

typedef enum {
    ARCO_MINOR_CHORD = 0,
    ARCO_MAJOR_CHORD = 1,
    ARCO_SEVENTH_CHORD = 2
} ArcoChordType;

typedef struct {
	// Features
	LV2_URID_Map*  map;
	LV2_Log_Logger logger;

	// Ports
	const LV2_Atom_Sequence* in_port;
	LV2_Atom_Sequence* out_port;
	float* arp_speed_port;
	float* arp_reverse_port;
	float* arp_enable_port;
	float* chord_enable_port;
	float* chord_port;
	float* octave_port;
	float* arp_pattern_port;

	// URIs
	ArcoURIs uris;

	// Arp
	int current_note, last_note_value;
	int time;
	float rate;
	SortedSet notes;
	Chord chord_list[3];

	unsigned int note_counter;
	
	uint8_t note_count[128];
	uint8_t note_vel[128];
	uint8_t note_channel[128];
	uint32_t active_count;
	bool    gen_active[128];
	uint8_t gen_channel[128];
	uint8_t gen_vel[128];
} Arco;

// Struct for a 3 byte MIDI event, used for writing notes
typedef struct {
    LV2_Atom_Event event;
    uint8_t msg[3];
} MIDINoteEvent;

#endif /* ARCO_H */