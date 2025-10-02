#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
@class arco_script_handler;

@interface arco_window : NSObject
@property (nonatomic, strong) NSWindow* window;
@property (nonatomic, strong) WKWebView* webView;

- (instancetype)initWithUI:(void*)ui htmlContent:(const char*)html;
@end
