#include "arp.h"

void arco_run_arp(Arco* self, uint32_t sample_count, ArcoChordType chord_type, int pattern_type) {
    ArcoURIs* uris = &self->uris;
    const uint32_t out_capacity = self->out_port->atom.size;

    lv2_atom_sequence_clear(self->out_port);
    self->out_port->atom.type = self->in_port->atom.type;

	int base_duration = ceil(self->rate * 0.25f * (0.1f + (1.0f - (*self->arp_speed_port))));

	int note_duration = base_duration;  

	if (pattern_type == 1) {
		if (self->note_counter % 3 == 2)
			note_duration = base_duration * 2;  
	}
	else if (pattern_type == 2) {
		int step = self->note_counter % 3;
		if (step == 0) note_duration = base_duration * 0.5;
		else if (step == 1) note_duration = base_duration * 1;
		else note_duration = base_duration * 1.5;
	}

    LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
		if (ev->body.type == uris->midi_Event) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
			unsigned char num_notes = self->chord_list[chord_type].size;
            switch (lv2_midi_message_type(msg)) {
                case LV2_MIDI_MSG_NOTE_ON:
					for(int j=0;j<=*self->octave_port;++j) {
						ss_add(&self->notes, msg[1] + (j * 12));
						if(*self->chord_enable_port > 0.5f && msg[1] <= 127 - self->chord_list[chord_type].notes[num_notes - 1]) {
							for(int i=0;i<num_notes;++i)
								ss_add(&self->notes, msg[1] + self->chord_list[chord_type].notes[i] + (j * 12));
						}
					}
					break;
                case LV2_MIDI_MSG_NOTE_OFF:
					for(int j=0;j<=*self->octave_port;++j) {
						ss_remove(&self->notes, msg[1] + (j * 12));
						if(*self->chord_enable_port > 0.5f && msg[1] <= 127 - self->chord_list[chord_type].notes[num_notes - 1]) {
							for(int i=0;i<num_notes;++i)
								ss_remove(&self->notes, msg[1] + self->chord_list[chord_type].notes[i] + (j * 12));
						}
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
			++self->note_counter;
        }
	}
	self->time = (self->time + sample_count) % note_duration;
}