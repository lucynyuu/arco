#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <lv2/ui/ui.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_TYPE parent
#define EG_ARCO_URI "https://example.org/plugins/arco#ui"

typedef struct {
    Display* display;
    Window   window;
    GC       gc;
} ArcoUI;

static void draw(ArcoUI* ui) {
    XDrawString(ui->display, ui->window, ui->gc,
                50, 50, "Hello, World!", 13);
    XFlush(ui->display);
}

static LV2UI_Handle instantiate(const LV2UI_Descriptor* descriptor,
        const char* plugin_uri,
        const char* bundle_path,
        LV2UI_Write_Function write_function,
        LV2UI_Controller controller,
        LV2UI_Widget* widget,
        const LV2_Feature* const* features)
{
    ArcoUI* ui = (ArcoUI*)calloc(1, sizeof(ArcoUI));
    if (!ui) {
         return NULL;
    }

    Window parent = 0;
    for (int i = 0; features && features[i]; ++i) {
        if (!strcmp(features[i]->URI, LV2_UI__parent)) {
            parent = (Window)features[i]->data;
            break;
        }
    }

    if (!parent) {
        fprintf(stderr, "error: UI requires a parent window\n");
        free(ui);
        return NULL;
    }

    ui->display = XOpenDisplay(NULL);
    if (!ui->display) {
        fprintf(stderr, "error: Failed to open X display\n");
        free(ui);
        return NULL;
    }

    int screen = DefaultScreen(ui->display);
    
    ui->window = XCreateSimpleWindow(
        ui->display,
        WINDOW_TYPE,
        0, 0, 200, 200, 0,
        BlackPixel(ui->display, screen),
        WhitePixel(ui->display, screen)
    );
    
    XSizeHints* hints = XAllocSizeHints();
    if (hints) {
        hints->flags = PMinSize;
        hints->min_width = 200;
        hints->min_height = 200;
        XSetWMNormalHints(ui->display, ui->window, hints);
        XFree(hints);
    }

    XSelectInput(ui->display, ui->window, ExposureMask);

    XMapWindow(ui->display, ui->window);
    ui->gc = XCreateGC(ui->display, ui->window, 0, NULL);

    XFlush(ui->display);

    *widget = (LV2UI_Widget)ui->window;
    return ui;
}

static void cleanup(LV2UI_Handle handle) {
    ArcoUI* ui = (ArcoUI*)handle;
    if (ui) {
        XFreeGC(ui->display, ui->gc);
        XDestroyWindow(ui->display, ui->window);
        XCloseDisplay(ui->display);
        free(ui);
    }
}

static int idle(LV2UI_Handle handle) {
    ArcoUI* ui = (ArcoUI*)handle;
    XEvent ev;
    while (XPending(ui->display)) {
        XNextEvent(ui->display, &ev);
        if (ev.type == Expose) {
            draw(ui);
        }
    }
    return 0;
}

static const LV2UI_Idle_Interface idle_iface = { idle };

static const void* extension_data(const char* uri) {
    if (!strcmp(uri, LV2_UI__idleInterface)) {
        return &idle_iface;
    }
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