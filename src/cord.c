#include "cord.h"

void arco_run_cord(Arco* self, uint32_t sample_count, ArcoChordType chord_type) {
    ArcoURIs* uris = &self->uris;
	const uint32_t out_capacity = self->out_port->atom.size;

	lv2_atom_sequence_clear(self->out_port);
	self->out_port->atom.type = self->in_port->atom.type;

	LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
			const uint8_t* const msg = (const uint8_t*)(ev + 1);
			switch (lv2_midi_message_type(msg)) {
				case LV2_MIDI_MSG_NOTE_ON:
				case LV2_MIDI_MSG_NOTE_OFF:
					lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

					if (msg[1] <= 127 - 7) {
						MIDINoteEvent major_third;
						major_third.event.time.frames = ev->time.frames;
						major_third.event.body.type   = ev->body.type;
						major_third.event.body.size   = ev->body.size;
						major_third.msg[0] = msg[0];
						major_third.msg[1] = msg[1] + chord_type;
						major_third.msg[2] = msg[2];
						lv2_atom_sequence_append_event(self->out_port, out_capacity, &major_third.event);

						MIDINoteEvent fifth;
						fifth.event.time.frames = ev->time.frames;
						fifth.event.body.type   = ev->body.type;
						fifth.event.body.size   = ev->body.size;
						fifth.msg[0] = msg[0];
						fifth.msg[1] = msg[1] + 7;
						fifth.msg[2] = msg[2];
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
