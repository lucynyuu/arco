#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <lv2/ui/ui.h>

#include <lv2/urid/urid.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/forge.h>
#include <lv2/atom/util.h>

typedef struct {
    NSWindow* window;
    WKWebView* webView;
    NSView* dummyView;

    LV2UI_Write_Function write_function;
    LV2UI_Controller controller;
    LV2_URID_Map* map;
    LV2_URID atom_Float;

    LV2_URID arp_speed_key;
    LV2_URID arp_reverse_key;
} ArcoUI;

const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index);
