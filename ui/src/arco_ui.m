#import "arco_ui.h"
#import "arco_window.h"
#include "gui_html.h"

#define EG_ARCO_URI "https://example.org/plugins/arco#ui"

static LV2UI_Handle instantiate(const LV2UI_Descriptor* descriptor,
                                const char* plugin_uri,
                                const char* bundle_path,
                                LV2UI_Write_Function write_function,
                                LV2UI_Controller controller,
                                LV2UI_Widget* widget,
                                const LV2_Feature* const* features)
{
    @autoreleasepool {
        if (![NSApp isRunning]) {
            [NSApplication sharedApplication];
        }

        ArcoUI* ui = calloc(1, sizeof(ArcoUI));
        if (!ui) return NULL;

        ui->write_function = write_function;
        ui->controller = controller;

        for (const LV2_Feature* const* f = features; *f; ++f) {
            if (!strcmp((*f)->URI, LV2_URID__map)) {
                ui->map = (LV2_URID_Map*)(*f)->data;
            }
        }
        if (ui->map) {
            ui->atom_Float = ui->map->map(ui->map->handle, LV2_ATOM__Float);
            ui->arp_speed_key = ui->map->map(ui->map->handle, "https://example.org/arco#arp_speed");
            ui->arp_reverse_key = ui->map->map(ui->map->handle, "https://example.org/arco#arp_reverse");
            ui->arp_enable_key = ui->map->map(ui->map->handle, "https://example.org/arco#arp_enable");
            ui->chord_enable_key = ui->map->map(ui->map->handle, "https://example.org/arco#chord_enable");
            ui->chord_key = ui->map->map(ui->map->handle, "https://example.org/arco#chord");
            ui->octave_key = ui->map->map(ui->map->handle, "https://example.org/arco#octave");
            ui->arp_pattern_key = ui->map->map(ui->map->handle, "https://example.org/arco#arp_pattern");
        }

        ui->dummyView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)];
        *widget = (LV2UI_Widget)ui->dummyView;

        arco_window* arcoWin = [[arco_window alloc] initWithUI:ui htmlContent:HTML_CONTENT];
        ui->window = arcoWin.window;
        ui->webView = arcoWin.webView;

        return ui;
    }
}

static void cleanup(LV2UI_Handle handle) {
    @autoreleasepool {
        ArcoUI* ui = (ArcoUI*)handle;
        if (!ui) return;

        if (ui->window) [ui->window close];
        if (ui->dummyView) [ui->dummyView release];
        free(ui);
    }
}

static const void* extension_data(const char* uri) {
    return NULL;
}

static const LV2UI_Descriptor descriptor = {
    EG_ARCO_URI,
    instantiate,
    cleanup,
    NULL,
    extension_data
};

LV2_SYMBOL_EXPORT const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index) {
    return (index == 0) ? &descriptor : NULL;
}
