#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <lv2/ui/ui.h>

#define EG_ARCO_URI "https://example.org/plugins/arco#ui"

typedef struct {
    NSWindow*   window;
    WKWebView*  webView;
    NSView*     dummyView;   // JUCE sees this
    id          delegate;    // window delegate
} ArcoUI;

// --- Delegate to stop crashes when user clicks the red close button ---
@interface ArcoWindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation ArcoWindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    [sender orderOut:nil]; // just hide, don’t free
    return NO;
}
@end

// --- DummyView that reopens our real window when host "shows" the GUI ---
@interface DummyView : NSView
@property (nonatomic, assign) NSWindow* externalWindow;
@end

@implementation DummyView
- (void)setHidden:(BOOL)hidden {
    [super setHidden:hidden];
    if (!hidden && self.externalWindow) {
        [self.externalWindow makeKeyAndOrderFront:nil];
    }
}
@end

// --- Instantiate the UI ---
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

        // 1. Create the real floating window
        ui->window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 400, 300)
                                                 styleMask:(NSWindowStyleMaskTitled |
                                                            NSWindowStyleMaskClosable |
                                                            NSWindowStyleMaskMiniaturizable |
                                                            NSWindowStyleMaskResizable)
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
        [ui->window setTitle:@"LV2 Cocoa WebUI"];

        // Attach delegate to intercept close
        ArcoWindowDelegate* delegate = [[ArcoWindowDelegate alloc] init];
        [ui->window setDelegate:delegate];
        ui->delegate = delegate;

        // Add WKWebView inside
        WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
        ui->webView = [[WKWebView alloc] initWithFrame:[[ui->window contentView] bounds]
                                          configuration:config];
        [ui->webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [[ui->window contentView] addSubview:ui->webView];

        NSString* html = @"<html><body style='font-family:sans-serif; text-align:center; margin-top:100px;'>"
                         "<h1>Hello, LV2 Window!</h1>"
                         "<p>This window can reopen after close.</p>"
                         "</body></html>";
        [ui->webView loadHTMLString:html baseURL:nil];

        [ui->window makeKeyAndOrderFront:nil];

        // 2. Create DummyView for JUCE
        DummyView* dummy = [[DummyView alloc] initWithFrame:NSMakeRect(0, 0, 1, 1)];
        dummy.externalWindow = ui->window;
        ui->dummyView = dummy;
        *widget = (LV2UI_Widget)dummy;

        return ui;
    }
}

// --- Cleanup ---
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
