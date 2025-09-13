#include "arp.h"

void arco_run_arp(Arco* self, uint32_t sample_count) {
    ArcoURIs* uris = &self->uris;
    const uint32_t out_capacity = self->out_port->atom.size;

    // Clear output buffer
    lv2_atom_sequence_clear(self->out_port);
    self->out_port->atom.type = self->in_port->atom.type;

    LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev) {
        if (ev->body.type == uris->midi_Event) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);

            switch (lv2_midi_message_type(msg)) {
                case LV2_MIDI_MSG_NOTE_ON: {
                    // Schedule arp notes
                    for (int i = 0; i < arp_steps; ++i) {
                        MIDINoteEvent note;
                        note.event.time.frames = ev->time.frames + (i * arp_spacing);
                        note.event.body.type   = ev->body.type;
                        note.event.body.size   = ev->body.size;

                        note.msg[0] = msg[0];                       // same status
                        note.msg[1] = msg[1] + arp_intervals[i];    // shifted pitch
                        note.msg[2] = msg[2];                       // same velocity

                        lv2_atom_sequence_append_event(self->out_port, out_capacity, &note.event);
                    }
                } break;

                case LV2_MIDI_MSG_NOTE_OFF: {
                    // For now just pass NOTE_OFF through directly
                    lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
                } break;

                default:
                    // Forward other MIDI events directly
                    lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
                break;
            }
        }
    }
}