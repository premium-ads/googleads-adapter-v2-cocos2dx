#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import <string>
#import "../../../Classes/AdMobBridge/Rewarded.h"

static GADRewardedAd *sAd = nil;

@interface PACRewardedDelegate : NSObject <GADFullScreenContentDelegate>
@end
@implementation PACRewardedDelegate
- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad { admob::Rewarded::onPlatformEvent(2, ""); }
- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad { admob::Rewarded::onPlatformEvent(3, ""); }
- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad { admob::Rewarded::onPlatformEvent(4, ""); }
- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)err {
    admob::Rewarded::onPlatformEvent(5, err.localizedDescription.UTF8String ?: "");
}
- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad { [sAd release]; sAd = nil; admob::Rewarded::onPlatformEvent(6, ""); }
@end
static PACRewardedDelegate *sDelegate = nil;

extern "C" void admob_ios_rewarded_load(const char* adUnitId) {
    [GADRewardedAd loadWithAdUnitID:[NSString stringWithUTF8String:adUnitId]
                            request:[GADRequest request]
                  completionHandler:^(GADRewardedAd *ad, NSError *err) {
        if (err) { [sAd release]; sAd = nil; admob::Rewarded::onPlatformEvent(1, err.localizedDescription.UTF8String ?: ""); return; }
        [sAd release];
        sAd = [ad retain];   // MRC retain
        if (!sDelegate) sDelegate = [PACRewardedDelegate new];
        sAd.fullScreenContentDelegate = sDelegate;
        admob::Rewarded::onPlatformEvent(0, "");
    }];
}

extern "C" bool admob_ios_rewarded_isReady() { return sAd != nil; }

extern "C" void admob_ios_rewarded_show() {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = UIApplication.sharedApplication.delegate.window.rootViewController;
        if (!sAd || !vc) { admob::Rewarded::onPlatformEvent(5, "no ad ready"); return; }
        [sAd presentFromRootViewController:vc userDidEarnRewardHandler:^{
            GADAdReward *r = sAd.adReward;
            std::string msg = std::string(r.type.UTF8String ?: "") + "|" + std::to_string(r.amount.intValue);
            admob::Rewarded::onPlatformEvent(7, msg);
        }];
    });
}
