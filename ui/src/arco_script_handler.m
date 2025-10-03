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
    if ([message.name isEqualToString:@"arpReverse"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    3,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
    if ([message.name isEqualToString:@"arpEnable"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    4,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
    if ([message.name isEqualToString:@"chordEnable"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    5,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
    if ([message.name isEqualToString:@"chord"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    6,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
    if ([message.name isEqualToString:@"octave"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    7,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
    if ([message.name isEqualToString:@"arpPattern"]) {
        NSNumber* num = message.body;
        float v = num.floatValue;

        if (self.ui && self.ui->write_function) {
            self.ui->write_function(self.ui->controller,
                                    8,
                                    sizeof(float),
                                    0,
                                    &v);
        }
    }
}

@end
