#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <lv2/ui/ui.h>

#define EG_ARCO_URI "https://example.org/plugins/arco#ui"

typedef struct {
    NSWindow*   window;
    WKWebView*  webView;
    NSView*     dummyView;   // what JUCE host actually sees
    id          delegate;    // retain delegate
} ArcoUI;

// Delegate to stop crashes when user clicks the red close button
@interface ArcoWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation ArcoWindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    [sender orderOut:nil]; // just hide instead of destroying
    return NO;             // don’t let Cocoa actually free it
}
@end

// Instantiate the UI
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

        // 1. Give JUCE a tiny invisible NSView so it doesn't crash
        ui->dummyView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 1, 1)];
        *widget = (LV2UI_Widget)ui->dummyView;

        // 2. Create your real floating NSWindow
        ui->window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 400, 300)
                                                 styleMask:(NSWindowStyleMaskTitled |
                                                            NSWindowStyleMaskClosable |
                                                            NSWindowStyleMaskMiniaturizable |
                                                            NSWindowStyleMaskResizable)
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
        [ui->window setTitle:@"LV2 Cocoa WebUI"];

        // 3. Attach delegate to intercept "close"
        ArcoWindowDelegate* delegate = [[ArcoWindowDelegate alloc] init];
        [ui->window setDelegate:delegate];
        ui->delegate = delegate; // keep alive

        // 4. Add WKWebView
        WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
        ui->webView = [[WKWebView alloc] initWithFrame:[[ui->window contentView] bounds]
                                          configuration:config];
        [ui->webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [[ui->window contentView] addSubview:ui->webView];

        NSString* html = @"<html><body style='font-family:sans-serif; text-align:center; margin-top:100px;'>"
                         "<h1>Hello, LV2 Window!</h1>"
                         "<p>This is a standalone Cocoa window.</p>"
                         "</body></html>";
        [ui->webView loadHTMLString:html baseURL:nil];

        [ui->window makeKeyAndOrderFront:nil];

        return ui;
    }
}

// Cleanup
static void cleanup(LV2UI_Handle handle) {
    @autoreleasepool {
        ArcoUI* ui = (ArcoUI*)handle;
        if (ui) {
            if (ui->webView) [ui->webView release];
            if (ui->window) {
                [ui->window close]; // hides + releases
                [ui->window release];
            }
            if (ui->dummyView) [ui->dummyView release];
            if (ui->delegate) [ui->delegate release];
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
