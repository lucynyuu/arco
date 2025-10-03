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

            if (midi_type == LV2_MIDI_MSG_NOTE_ON && msg[2] > 0) {
                uint8_t note = msg[1];
                uint8_t vel  = msg[2];
                uint8_t channel = msg[0] & 0x0F;

                if (self->note_count[note] == 0) self->active_count++;
                if (self->note_count[note] < 255) self->note_count[note]++;
                self->note_vel[note] = vel;
                self->note_channel[note] = channel;

                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

                if (old_active == 0 && self->active_count == 1) {
                    unsigned char num_notes = self->chord_list[chord_type].size;
                    uint8_t highest_interval = self->chord_list[chord_type].notes[num_notes - 1];
                    if (note <= 127 - highest_interval) {
                        for (int i = 0; i < num_notes; ++i) {
                            uint8_t interval = self->chord_list[chord_type].notes[i];
                            if (interval == 0) continue;
                            uint8_t pitch = note + interval;
                            if (self->note_count[pitch] == 0 && !self->gen_active[pitch]) {
                                MIDINoteEvent note_ev;
                                note_ev.event.time.frames = ev->time.frames;
                                note_ev.event.body.type = ev->body.type;
                                note_ev.event.body.size = ev->body.size;
                                note_ev.msg[0] = 0x90 | (channel & 0x0F);
                                note_ev.msg[1] = pitch;
                                note_ev.msg[2] = vel;
                                lv2_atom_sequence_append_event(self->out_port, out_capacity, &note_ev.event);

                                self->gen_active[pitch] = true;
                                self->gen_channel[pitch] = channel;
                                self->gen_vel[pitch] = vel;
                            }
                        }
                    }
                } else if (old_active == 1 && self->active_count == 2) {
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
            }
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

                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);

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
                } else if (old_active >= 2 && self->active_count == 1) {
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
            } else {
                lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
            }
        } else {
            lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
        }
    }
}
