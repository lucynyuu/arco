/*
#include "chord.h"

void arco_run_chord(Arco* self, uint32_t sample_count, ArcoChordType chord_type) {
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
					unsigned char num_notes = self->chord_list[chord_type].size;
					if (msg[1] <= 127 - self->chord_list[chord_type].notes[num_notes - 1]) {
						for(int i=0;i<num_notes;++i) {
							MIDINoteEvent note;
							note.event.time.frames = ev->time.frames;
							note.event.body.type   = ev->body.type;
							note.event.body.size   = ev->body.size;
							note.msg[0] = msg[0];
							note.msg[1] = msg[1] + self->chord_list[chord_type].notes[i];
							note.msg[2] = msg[2];
							lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
						}
					}
					break;
				default:
					lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
				break;
			}
		}
	}
}*/

#include "chord.h"

void arco_run_chord(Arco* self, uint32_t sample_count, ArcoChordType chord_type)
{
    ArcoURIs* uris = &self->uris;
    const uint32_t out_capacity = self->out_port->atom.size;

    lv2_atom_sequence_clear(self->out_port);
    self->out_port->atom.type = self->in_port->atom.type;

    LV2_ATOM_SEQUENCE_FOREACH(self->in_port, ev) {
        if (ev->body.type == uris->midi_Event) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            uint8_t midi_type = lv2_midi_message_type(msg);

            uint32_t old_active = self->active_count;

            /* NOTE ON (vel > 0) */
            if (midi_type == LV2_MIDI_MSG_NOTE_ON && msg[2] > 0) {
                uint8_t note = msg[1];
                uint8_t vel  = msg[2];
                uint8_t channel = msg[0] & 0x0F;

                /* Update per-note counts/metadata */
                if (self->note_count[note] == 0) self->active_count++;
                if (self->note_count[note] < 255) self->note_count[note]++; /* prevent overflow */
                self->note_vel[note] = vel;
                self->note_channel[note] = channel;

                /* Always forward original event */
                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

                /* Transition: 0 -> 1  (become single note) => generate chord */
                if (old_active == 0 && self->active_count == 1) {
                    unsigned char num_notes = self->chord_list[chord_type].size;
                    uint8_t highest_interval = self->chord_list[chord_type].notes[num_notes - 1];
                    if (note <= 127 - highest_interval) {
                        for (int i = 0; i < num_notes; ++i) {
                            uint8_t interval = self->chord_list[chord_type].notes[i];
                            if (interval == 0) continue; /* original root already forwarded */
                            uint8_t pitch = note + interval;
                            /* don't generate if a real note at that pitch already exists */
                            if (self->note_count[pitch] == 0 && !self->gen_active[pitch]) {
                                MIDINoteEvent note_ev;
                                note_ev.event.time.frames = ev->time.frames;
                                note_ev.event.body.type = ev->body.type;
                                note_ev.event.body.size = ev->body.size;
                                note_ev.msg[0] = 0x90 | (channel & 0x0F); /* Note On on same channel */
                                note_ev.msg[1] = pitch;
                                note_ev.msg[2] = vel;
                                lv2_atom_sequence_append_event(self->out_port, out_capacity, &note_ev.event);

                                self->gen_active[pitch] = true;
                                self->gen_channel[pitch] = channel;
                                self->gen_vel[pitch] = vel;
                            }
                        }
                    }
                }
                /* Transition: 1 -> 2 (second real note pressed) => kill any generated notes */
                else if (old_active == 1 && self->active_count == 2) {
                    for (int p = 0; p < 128; ++p) {
                        if (self->gen_active[p]) {
                            MIDINoteEvent off_ev;
                            off_ev.event.time.frames = ev->time.frames;
                            off_ev.event.body.type = ev->body.type;
                            off_ev.event.body.size = ev->body.size;
                            off_ev.msg[0] = 0x80 | (self->gen_channel[p] & 0x0F); /* Note Off on stored channel */
                            off_ev.msg[1] = (uint8_t)p;
                            off_ev.msg[2] = 0;
                            lv2_atom_sequence_append_event(self->out_port, out_capacity, &off_ev.event);
                            self->gen_active[p] = false;
                        }
                    }
                }
            }
            /* NOTE OFF (or Note On with vel==0) */
            else if (midi_type == LV2_MIDI_MSG_NOTE_OFF || (midi_type == LV2_MIDI_MSG_NOTE_ON && msg[2] == 0)) {
                uint8_t note = msg[1];

                if (self->note_count[note] > 0) {
                    self->note_count[note]--;
                    if (self->note_count[note] == 0) {
                        self->active_count--;
                        self->note_vel[note] = 0;
                        self->note_channel[note] = 0;
                    }
                }

                /* Forward original off */
                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

                /* Transition: 1 -> 0 (released last real note) => kill generated notes */
                if (old_active == 1 && self->active_count == 0) {
                    for (int p = 0; p < 128; ++p) {
                        if (self->gen_active[p]) {
                            MIDINoteEvent off_ev;
                            off_ev.event.time.frames = ev->time.frames;
                            off_ev.event.body.type = ev->body.type;
                            off_ev.event.body.size = ev->body.size;
                            off_ev.msg[0] = 0x80 | (self->gen_channel[p] & 0x0F);
                            off_ev.msg[1] = (uint8_t)p;
                            off_ev.msg[2] = 0;
                            lv2_atom_sequence_append_event(self->out_port, out_capacity, &off_ev.event);
                            self->gen_active[p] = false;
                        }
                    }
                }
                /* Transition: >=2 -> 1 (one of many released) => start generating chord for remaining note */
                else if (old_active >= 2 && self->active_count == 1) {
                    int remaining = -1;
                    for (int i = 0; i < 128; ++i) {
                        if (self->note_count[i] > 0) { remaining = i; break; }
                    }
                    if (remaining >= 0) {
                        unsigned char num_notes = self->chord_list[chord_type].size;
                        uint8_t highest_interval = self->chord_list[chord_type].notes[num_notes - 1];
                        if (remaining <= 127 - highest_interval) {
                            uint8_t rem_vel = self->note_vel[remaining];
                            uint8_t rem_channel = self->note_channel[remaining] & 0x0F;
                            for (int i = 0; i < num_notes; ++i) {
                                uint8_t interval = self->chord_list[chord_type].notes[i];
                                if (interval == 0) continue;
                                uint8_t pitch = remaining + interval;
                                if (self->note_count[pitch] == 0 && !self->gen_active[pitch]) {
                                    MIDINoteEvent note_ev;
                                    note_ev.event.time.frames = ev->time.frames;
                                    note_ev.event.body.type = ev->body.type;
                                    note_ev.event.body.size = ev->body.size;
                                    note_ev.msg[0] = 0x90 | (rem_channel & 0x0F);
                                    note_ev.msg[1] = pitch;
                                    note_ev.msg[2] = rem_vel;
                                    lv2_atom_sequence_append_event(self->out_port, out_capacity, &note_ev.event);
                                    self->gen_active[pitch] = true;
                                    self->gen_channel[pitch] = rem_channel;
                                    self->gen_vel[pitch] = rem_vel;
                                }
                            }
                        }
                    }
                }
            }
            else {
                /* Other MIDI messages - pass through */
                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
            }
        }
        else {
            /* Non-MIDI - pass through */
            lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
        }
    } /* LV2_ATOM_SEQUENCE_FOREACH */
}
