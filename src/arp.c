#include "arp.h"

void arco_run_arp(Arco* self, uint32_t sample_count) {
    ArcoURIs* uris = &self->uris;
    const uint32_t out_capacity = self->out_port->atom.size;

    lv2_atom_sequence_clear(self->out_port);
    self->out_port->atom.type = self->in_port->atom.type;

	int noteDuration = ceil(self->rate * 0.25f * (0.1f + (1.0f - (self->arpSpeed))));

    LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            switch (lv2_midi_message_type(msg)) {
                case LV2_MIDI_MSG_NOTE_ON:
					set_note(msg[1]);
					break;
                case LV2_MIDI_MSG_NOTE_OFF:
					clear_note(msg[1]);
					break;
                default:
                    lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
                    break;
            }
        }
    }
	lv2_atom_sequence_clear(self->out_port);
	if((self->time + sample_count) >= noteDuration) {
		int64_t offset = 118; // Placeholder
        if (self->lastNoteValue > 0) {
			MIDINoteEvent ev;
			ev.event.time.frames = offset;
			ev.event.body.type   = 27; // Placeholder
			ev.event.body.size   = 3;  // Placeholder
			ev.msg[0] = LV2_MIDI_MSG_NOTE_OFF;
			ev.msg[1] = self->lastNoteValue;
			ev.msg[2] = 0;
			lv2_atom_sequence_append_event(self->out_port, out_capacity, &ev.event);
            self->lastNoteValue = -1;
        }
        if (sum_notes()>0) {
			self->currentNote = next_note_shit(self->currentNote);
            self->lastNoteValue = self->currentNote;

			MIDINoteEvent ev;
			ev.event.time.frames = offset;
			ev.event.body.type   = 27; // Placeholder
			ev.event.body.size   = 3;  // Placeholder
			ev.msg[0] = LV2_MIDI_MSG_NOTE_ON;
			ev.msg[1] = self->lastNoteValue;
			ev.msg[2] = 127;
			lv2_atom_sequence_append_event(self->out_port, out_capacity, &ev.event);
        }
	}
	self->time = (self->time + sample_count) % noteDuration;
}