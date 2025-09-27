#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (strong) NSWindow *window;
@property (strong) WKWebView *webView;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Create window
    self.window = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 600, 400)
                                              styleMask:(NSWindowStyleMaskTitled |
                                                         NSWindowStyleMaskClosable |
                                                         NSWindowStyleMaskResizable |
                                                         NSWindowStyleMaskMiniaturizable)
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:@"Arco Standalone UI"];

    // Create webview
    WKWebViewConfiguration *config = [[WKWebViewConfiguration alloc] init];
    self.webView = [[WKWebView alloc] initWithFrame:[[self.window contentView] bounds]
                                        configuration:config];
    [self.webView setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [[self.window contentView] addSubview:self.webView];

    // Load local HTML file
    NSString *htmlPath = [[NSBundle mainBundle] pathForResource:@"index" ofType:@"html"];
    if (htmlPath) {
        NSURL *url = [NSURL fileURLWithPath:htmlPath];
        [self.webView loadFileURL:url allowingReadAccessToURL:[url URLByDeletingLastPathComponent]];
    } else {
        // Fallback: inline HTML
        NSString *html = @"<html><body style='font-family:sans-serif; text-align:center; margin-top:100px;'>"
                         "<h1>Hello from Standalone!</h1>"
                         "<p>Edit index.html, CSS, and JS here without relaunching pluginhost.</p>"
                         "</body></html>";
        [self.webView loadHTMLString:html baseURL:nil];
    }

    [self.window makeKeyAndOrderFront:nil];
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        return NSApplicationMain(argc, argv);
    }
}
