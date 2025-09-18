#include "cord.h"

void arco_run_cord(Arco* self, uint32_t sample_count, ArcoChordType chord_type) {
    ArcoURIs* uris = &self->uris;
	const uint32_t out_capacity = self->out_port->atom.size;

	lv2_atom_sequence_clear(self->out_port);
	self->out_port->atom.type = self->in_port->atom.type;

	FILE *fptr;
	fptr = fopen("/home/Nyuu/Documents/university/Y4-T1/LE-EECS-4462/A1/debugstuff.txt", "a+");

	LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
			const uint8_t* const msg = (const uint8_t*)(ev + 1);
			fprintf(fptr, "Status: %d\nNote: %d\nVelocity: %d\nFrames: %d\nType: %d\nSize: %d\n\n", msg[0], msg[1], msg[2], ev->time.frames, ev->body.type, ev->body.size);
			switch (lv2_midi_message_type(msg)) {
				case LV2_MIDI_MSG_NOTE_ON:
				case LV2_MIDI_MSG_NOTE_OFF:
					// Forward note to output
					lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

					if (msg[1] <= 127 - 7) {
						MIDINoteEvent major_third;
						major_third.event.time.frames = ev->time.frames; // Same time
						major_third.event.body.type   = ev->body.type;   // Same type
						major_third.event.body.size   = ev->body.size;   // Same size
						major_third.msg[0] = msg[0];     				 // Same status
						major_third.msg[1] = msg[1] + chord_type; 		 // Pitch up 4 semitones
						major_third.msg[2] = msg[2];     				 // Same velocity
						lv2_atom_sequence_append_event(self->out_port, out_capacity, &major_third.event);

						MIDINoteEvent fifth;
						fifth.event.time.frames = ev->time.frames; // Same time
						fifth.event.body.type   = ev->body.type;   // Same type
						fifth.event.body.size   = ev->body.size;   // Same size
						fifth.msg[0] = msg[0];      // Same status
						fifth.msg[1] = msg[1] + 7;  // Pitch up 7 semitones
						fifth.msg[2] = msg[2];      // Same velocity
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
	fclose(fptr);
}
