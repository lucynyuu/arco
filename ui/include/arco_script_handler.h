#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

#include "arco_ui.h"

@interface arco_script_handler : NSObject <WKScriptMessageHandler>
@property (assign) ArcoUI* ui;
@end
