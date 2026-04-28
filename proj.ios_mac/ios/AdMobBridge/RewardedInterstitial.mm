#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import <string>
#import "../../../Classes/AdMobBridge/RewardedInterstitial.h"

static GADRewardedInterstitialAd *sAd = nil;

@interface PACRewardedInterstitialDelegate : NSObject <GADFullScreenContentDelegate>
@end
@implementation PACRewardedInterstitialDelegate
- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad { admob::RewardedInterstitial::onPlatformEvent(2, ""); }
- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad { admob::RewardedInterstitial::onPlatformEvent(3, ""); }
- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad { admob::RewardedInterstitial::onPlatformEvent(4, ""); }
- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)err {
    admob::RewardedInterstitial::onPlatformEvent(5, err.localizedDescription.UTF8String ?: "");
}
- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad { [sAd release]; sAd = nil; admob::RewardedInterstitial::onPlatformEvent(6, ""); }
@end
static PACRewardedInterstitialDelegate *sDelegate = nil;

extern "C" void admob_ios_rewardedInterstitial_load(const char* adUnitId) {
    [GADRewardedInterstitialAd loadWithAdUnitID:[NSString stringWithUTF8String:adUnitId]
                                        request:[GADRequest request]
                              completionHandler:^(GADRewardedInterstitialAd *ad, NSError *err) {
        if (err) { [sAd release]; sAd = nil; admob::RewardedInterstitial::onPlatformEvent(1, err.localizedDescription.UTF8String ?: ""); return; }
        [sAd release];
        sAd = [ad retain];   // MRC retain
        if (!sDelegate) sDelegate = [PACRewardedInterstitialDelegate new];
        sAd.fullScreenContentDelegate = sDelegate;
        admob::RewardedInterstitial::onPlatformEvent(0, "");
    }];
}

extern "C" bool admob_ios_rewardedInterstitial_isReady() { return sAd != nil; }

extern "C" void admob_ios_rewardedInterstitial_show() {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = UIApplication.sharedApplication.delegate.window.rootViewController;
        if (!sAd || !vc) { admob::RewardedInterstitial::onPlatformEvent(5, "no ad ready"); return; }
        [sAd presentFromRootViewController:vc userDidEarnRewardHandler:^{
            GADAdReward *r = sAd.adReward;
            std::string msg = std::string(r.type.UTF8String ?: "") + "|" + std::to_string(r.amount.intValue);
            admob::RewardedInterstitial::onPlatformEvent(7, msg);
        }];
    });
}
