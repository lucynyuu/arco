#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <lv2/ui/ui.h>

#define EG_ARCO_URI "https://example.org/plugins/arco#ui"

typedef struct {
    NSWindow*   window;
    WKWebView*  webView;
} ArcoUI;

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
        
        ArcoUI* ui = (ArcoUI*)calloc(1, sizeof(ArcoUI));
        if (!ui) return NULL;

        // Create the window
        ui->window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 400, 300)
                                                 styleMask:(NSWindowStyleMaskTitled |
                                                            NSWindowStyleMaskClosable)
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
        [ui->window setTitle:@"LV2 Cocoa WebUI"];

        // Create a WKWebView
        WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
        ui->webView = [[WKWebView alloc] initWithFrame:[[ui->window contentView] bounds]
                                          configuration:config];
        [ui->webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [[ui->window contentView] addSubview:ui->webView];

        // Load some HTML directly
        NSString* html = @"<html><body style='font-family:sans-serif; text-align:center; margin-top:100px;'>"
                          "<h1>Hello, World!</h1>"
                          "</body></html>";
        [ui->webView loadHTMLString:html baseURL:nil];

        [ui->window makeKeyAndOrderFront:nil];

        *widget = (LV2UI_Widget)ui->window;
        return ui;
    }
}

static void cleanup(LV2UI_Handle handle) {
    @autoreleasepool {
        ArcoUI* ui = (ArcoUI*)handle;
        if (ui) {
            [ui->webView release];
            [ui->window release];
            free(ui);
        }
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
