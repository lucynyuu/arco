#include "arco.h"
#include "arp.h"
#include "fifths.h"
#include "chord.h"

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Arco* self = (Arco*)instance;
	switch (port) {
		case ARCO_IN:
			self->in_port = (const LV2_Atom_Sequence*)data;
			break;
		case ARCO_OUT:
			self->out_port = (LV2_Atom_Sequence*)data;
			break;
		case ARCO_CONTROL:
			self->arp_speed_port = (float*)data;
			break;
		case ARCO_REVERSE:
			self->arp_reverse_port = (float*)data;
			break;
		case ARCO_ARP_ENABLE:
			self->arp_enable_port = (float*)data;
			break;
		case ARCO_CHORD_ENABLE:
			self->chord_enable_port = (float*)data;
			break;
		case ARCO_CHORD:
			self->chord_port = (float*)data;
			break;
		case ARCO_OCTAVE:
			self->octave_port = (float*)data;
		default:
			break;
	}
}

static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double rate, const char* path, const LV2_Feature* const* features) {
	Arco* self = (Arco*)calloc(1, sizeof(Arco));
		if (!self) {
		return NULL;
	}

	const char*  missing = lv2_features_query(features, LV2_LOG__log,  &self->logger.log, false, LV2_URID__map, &self->map, true, NULL);

	lv2_log_logger_set_map(&self->logger, self->map);
	if (missing) {
		lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
		free(self);
		return NULL;
	}

	map_arco_uris(self->map, &self->uris);

	ss_clear(&self->notes);
	self->current_note = 0;
	self->last_note_value = -1;
	self->time = 0;
	self->rate = (float)rate;

	memset(self->note_count,  0, sizeof(self->note_count));
	memset(self->note_vel,    0, sizeof(self->note_vel));
	memset(self->note_channel,0, sizeof(self->note_channel));
	self->active_count = 0;
	memset(self->gen_active,  0, sizeof(self->gen_active));
	memset(self->gen_channel, 0, sizeof(self->gen_channel));
	memset(self->gen_vel,     0, sizeof(self->gen_vel));


	init_chord(&self->chord_list[0], 2, 4, 7, 0);
	init_chord(&self->chord_list[1], 2, 3, 7, 0);
	init_chord(&self->chord_list[2], 3, 4, 7, 10);

	return (LV2_Handle)self;
}

static void cleanup(LV2_Handle instance) {
	free(instance);
}

static void run(LV2_Handle instance, uint32_t sample_count) {
	Arco* self = (Arco*)instance;
	if(*self->arp_enable_port > 0.5f) {
		arco_run_arp(self, sample_count, *self->chord_port);
	} else if (*self->chord_enable_port > 0.5f) {
		arco_run_chord(self, sample_count, *self->chord_port);
	} else {
		ArcoURIs* uris = &self->uris;
		const uint32_t out_capacity = self->out_port->atom.size;
		lv2_atom_sequence_clear(self->out_port);
		self->out_port->atom.type = self->in_port->atom.type;
		LV2_ATOM_SEQUENCE_FOREACH (self->in_port, ev)
			if (ev->body.type == uris->midi_Event)
				lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
	}
}

static const void* extension_data(const char* uri) {
	return NULL;
}

static const LV2_Descriptor descriptor = {	EG_ARCO_URI,
											instantiate,
											connect_port,
											NULL, // activate,
											run,
											NULL, // deactivate,
											cleanup,
											extension_data};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index) {
	return index == 0 ? &descriptor : NULL;
}
