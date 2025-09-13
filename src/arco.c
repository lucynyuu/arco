#include "arco.h"
#include "arp.h"
#include "fifths.h"
#include "cord.h"

static void connect_port(LV2_Handle instance, uint32_t port, void* data) {
	Arco* self = (Arco*)instance;
	switch (port) {
		case ARCO_IN:
			self->in_port = (const LV2_Atom_Sequence*)data;
			break;
		case ARCO_OUT:
			self->out_port = (LV2_Atom_Sequence*)data;
			break;
		default:
			break;
	}
}

static LV2_Handle instantiate(const LV2_Descriptor* descriptor, double rate, const char* path, const LV2_Feature* const* features) {
	// Allocate and initialise instance structure.
	Arco* self = (Arco*)calloc(1, sizeof(Arco));
		if (!self) {
		return NULL;
	}

	// Scan host features for URID map
	// clang-format off
	const char*  missing = lv2_features_query(features, LV2_LOG__log,  &self->logger.log, false, LV2_URID__map, &self->map, true, NULL);
	// clang-format on

	lv2_log_logger_set_map(&self->logger, self->map);
	if (missing) {
		lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
		free(self);
		return NULL;
	}

	map_arco_uris(self->map, &self->uris);

	return (LV2_Handle)self;
}

static void cleanup(LV2_Handle instance) {
	free(instance);
}

static void run(LV2_Handle instance, uint32_t sample_count) {
	Arco* self = (Arco*)instance;
	arco_run_arp(self, sample_count);
	// arco_run_fifths(self, sample_count);
	// arco_run_cord(self, sample_count, ARCO_MAJOR_CHORD);
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
