#import "arco_script_handler.h"

@implementation arco_script_handler

- (void)userContentController:(WKUserContentController*)controller
      didReceiveScriptMessage:(WKScriptMessage*)message
{
    if ([message.name isEqualToString:@"arpSpeedChanged"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    2,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
}

@end
