#import "arco_window.h"
#import "arco_script_handler.h"

@implementation arco_window

- (instancetype)initWithUI:(void*)ui htmlContent:(const char*)html {
    self = [super init];
    if (self) {
        self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 600, 400)
                                                  styleMask:(NSWindowStyleMaskTitled |
                                                             NSWindowStyleMaskClosable)
                                                    backing:NSBackingStoreBuffered
                                                      defer:NO];
        [self.window setTitle:@"Arco GUI Controls"];

        WKWebViewConfiguration* config = [[WKWebViewConfiguration alloc] init];
        arco_script_handler* handler = [[arco_script_handler alloc] init];
        handler.ui = ui;
        [config.userContentController addScriptMessageHandler:handler name:@"arpSpeedChanged"];
        [config.userContentController addScriptMessageHandler:handler name:@"arpReverse"];
        [config.userContentController addScriptMessageHandler:handler name:@"arpEnable"];
        [config.userContentController addScriptMessageHandler:handler name:@"chordEnable"];
        [config.userContentController addScriptMessageHandler:handler name:@"chord"];
        [config.userContentController addScriptMessageHandler:handler name:@"octave"];
        [config.userContentController addScriptMessageHandler:handler name:@"arpPattern"];

        self.webView = [[WKWebView alloc] initWithFrame:self.window.contentView.bounds
                                          configuration:config];
        self.webView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        [self.window.contentView addSubview:self.webView];

        NSString* htmlString = [NSString stringWithUTF8String:html];
        [self.webView loadHTMLString:htmlString baseURL:nil];

        [self.window makeKeyAndOrderFront:nil];
    }
    return self;
}

@end
