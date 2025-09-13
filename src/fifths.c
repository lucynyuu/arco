#include "fifths.h"

void arco_run_fifths(Arco* self, uint32_t sample_count) {
    ArcoURIs* uris = &self->uris;

	// Struct for a 3 byte MIDI event, used for writing notes
	typedef struct {
		LV2_Atom_Event event;
		uint8_t        msg[3];
	} MIDINoteEvent;

	// Initially self->out_port contains a Chunk with size set to capacity

	// Get the capacity
	const uint32_t out_capacity = self->out_port->atom.size;

	// Write an empty Sequence header to the output
	lv2_atom_sequence_clear(self->out_port);
	self->out_port->atom.type = self->in_port->atom.type;

	// Read incoming events
	LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
			const uint8_t* const msg = (const uint8_t*)(ev + 1);
			switch (lv2_midi_message_type(msg)) {
				case LV2_MIDI_MSG_NOTE_ON:
				case LV2_MIDI_MSG_NOTE_OFF:
					// Forward note to output
					lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

					if (msg[1] <= 127 - 7) {
						// Make a note one 5th (7 semitones) higher than input
						MIDINoteEvent fifth;

						// Could simply do fifth.event = *ev here instead...
						fifth.event.time.frames = ev->time.frames; // Same time
						fifth.event.body.type   = ev->body.type;   // Same type
						fifth.event.body.size   = ev->body.size;   // Same size

						fifth.msg[0] = msg[0];     // Same status
						fifth.msg[1] = msg[1] + 7; // Pitch up 7 semitones
						fifth.msg[2] = msg[2];     // Same velocity

						// Write 5th event
						lv2_atom_sequence_append_event(self->out_port, out_capacity, &fifth.event);
					}
					break;
				default:
					// Forward all other MIDI events directly
					lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
				break;
			}
		}
	}
}