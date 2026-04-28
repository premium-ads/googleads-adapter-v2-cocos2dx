#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import "../../../Classes/AdMobBridge/AppOpen.h"

static GADAppOpenAd *sAd = nil;

@interface PACAppOpenDelegate : NSObject <GADFullScreenContentDelegate>
@end
@implementation PACAppOpenDelegate
- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad { admob::AppOpen::onPlatformEvent(2, ""); }
- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad { admob::AppOpen::onPlatformEvent(3, ""); }
- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad { admob::AppOpen::onPlatformEvent(4, ""); }
- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)err {
    admob::AppOpen::onPlatformEvent(5, err.localizedDescription.UTF8String ?: "");
}
- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad { [sAd release]; sAd = nil; admob::AppOpen::onPlatformEvent(6, ""); }
@end
static PACAppOpenDelegate *sDelegate = nil;

extern "C" void admob_ios_appOpen_load(const char* adUnitId, int orientation) {
    [GADAppOpenAd loadWithAdUnitID:[NSString stringWithUTF8String:adUnitId]
                            request:[GADRequest request]
                  completionHandler:^(GADAppOpenAd *ad, NSError *err) {
        if (err) { [sAd release]; sAd = nil; admob::AppOpen::onPlatformEvent(1, err.localizedDescription.UTF8String ?: ""); return; }
        [sAd release];
        sAd = [ad retain];   // MRC retain
        if (!sDelegate) sDelegate = [PACAppOpenDelegate new];
        sAd.fullScreenContentDelegate = sDelegate;
        admob::AppOpen::onPlatformEvent(0, "");
    }];
}

extern "C" bool admob_ios_appOpen_isReady() { return sAd != nil; }

extern "C" void admob_ios_appOpen_show() {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = UIApplication.sharedApplication.delegate.window.rootViewController;
        if (sAd && vc) [sAd presentFromRootViewController:vc];
        else admob::AppOpen::onPlatformEvent(5, "no ad ready");
    });
}
