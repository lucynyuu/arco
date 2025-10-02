#include "arp.h"

void arco_run_arp(Arco* self, uint32_t sample_count, ArcoChordType chord_type) {
    ArcoURIs* uris = &self->uris;
    const uint32_t out_capacity = self->out_port->atom.size;

    lv2_atom_sequence_clear(self->out_port);
    self->out_port->atom.type = self->in_port->atom.type;

	int note_duration = ceil(self->rate * 0.25f * (0.1f + (1.0f - (*self->arp_speed_port))));

    LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            switch (lv2_midi_message_type(msg)) {
                case LV2_MIDI_MSG_NOTE_ON:
					ss_add(&self->notes, msg[1]);
					//if(ss_size(&self->notes) == 1 && msg[1] <= 127 - 7) {
					//	ss_add(&self->notes, msg[1] + 4);
					//	ss_add(&self->notes, msg[1] + 7);
					//}
					if(msg[1] <= 127 - self->cord_array[chord_type][1]) {
						ss_add(&self->notes, msg[1] + self->cord_array[chord_type][0]);
						ss_add(&self->notes, msg[1] + self->cord_array[chord_type][1]);
					}
					break;
                case LV2_MIDI_MSG_NOTE_OFF:
					ss_remove(&self->notes, msg[1]);
					if(msg[1] <= 127 - self->cord_array[chord_type][1]) {
						ss_remove(&self->notes, msg[1] + self->cord_array[chord_type][0]);
						ss_remove(&self->notes, msg[1] + self->cord_array[chord_type][1]);
					}
					break;
                default:
                    lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
                    break;
            }
        }
    }
	lv2_atom_sequence_clear(self->out_port);
	if((self->time + sample_count) >= note_duration) {
		int offset = MAX(0, MIN(note_duration - self->time, sample_count - 1));
        if (self->last_note_value > 0) {
			MIDINoteEvent ev;
			ev.event.time.frames = offset;
			ev.event.body.type   = 27;
			ev.event.body.size   = 3;
			ev.msg[0] = LV2_MIDI_MSG_NOTE_OFF;
			ev.msg[1] = self->last_note_value;
			ev.msg[2] = 0;
			lv2_atom_sequence_append_event(self->out_port, out_capacity, &ev.event);
            self->last_note_value = -1;
        }
        if (self->notes.size > 0) {
			if (*self->arp_reverse_port > 0.5f)
				self->current_note = (self->current_note - 1 + ss_size(&self->notes)) % ss_size(&self->notes);
			else
				self->current_note = (self->current_note + 1) % ss_size(&self->notes);
			self->last_note_value = ss_get(&self->notes, self->current_note);

			MIDINoteEvent ev;
			ev.event.time.frames = offset;
			ev.event.body.type   = 27;
			ev.event.body.size   = 3;
			ev.msg[0] = LV2_MIDI_MSG_NOTE_ON;
			ev.msg[1] = self->last_note_value;
			ev.msg[2] = 127;
			lv2_atom_sequence_append_event(self->out_port, out_capacity, &ev.event);
        }
	}
	self->time = (self->time + sample_count) % note_duration;
}