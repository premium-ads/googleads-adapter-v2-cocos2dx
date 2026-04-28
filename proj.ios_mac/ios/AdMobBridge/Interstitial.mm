#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import "../../../Classes/AdMobBridge/Interstitial.h"

static GADInterstitialAd *sAd = nil;

@interface PACInterstitialDelegate : NSObject <GADFullScreenContentDelegate>
@end
@implementation PACInterstitialDelegate
- (void)adDidRecordImpression:(id<GADFullScreenPresentingAd>)ad { admob::Interstitial::onPlatformEvent(2, ""); }
- (void)adDidRecordClick:(id<GADFullScreenPresentingAd>)ad { admob::Interstitial::onPlatformEvent(3, ""); }
- (void)adWillPresentFullScreenContent:(id<GADFullScreenPresentingAd>)ad { admob::Interstitial::onPlatformEvent(4, ""); }
- (void)ad:(id<GADFullScreenPresentingAd>)ad didFailToPresentFullScreenContentWithError:(NSError *)err {
    admob::Interstitial::onPlatformEvent(5, err.localizedDescription.UTF8String ?: "");
}
- (void)adDidDismissFullScreenContent:(id<GADFullScreenPresentingAd>)ad { [sAd release]; sAd = nil; admob::Interstitial::onPlatformEvent(6, ""); }
@end
static PACInterstitialDelegate *sDelegate = nil;

extern "C" void admob_ios_interstitial_load(const char* adUnitId) {
    NSString *adId = [NSString stringWithUTF8String:adUnitId];
    [GADInterstitialAd loadWithAdUnitID:adId
                                request:[GADRequest request]
                      completionHandler:^(GADInterstitialAd *ad, NSError *err) {
        if (err) {
            [sAd release]; sAd = nil;
            admob::Interstitial::onPlatformEvent(1, err.localizedDescription.UTF8String ?: "");
            return;
        }
        [sAd release];
        sAd = [ad retain];   // MRC: project doesn't use ARC; static doesn't auto-retain block-captured autoreleased ad
        if (!sDelegate) sDelegate = [PACInterstitialDelegate new];
        sAd.fullScreenContentDelegate = sDelegate;
        admob::Interstitial::onPlatformEvent(0, "");
    }];
}

extern "C" bool admob_ios_interstitial_isReady() { return sAd != nil; }

extern "C" void admob_ios_interstitial_show() {
    dispatch_async(dispatch_get_main_queue(), ^{
        UIViewController *vc = UIApplication.sharedApplication.delegate.window.rootViewController;
        if (sAd && vc) [sAd presentFromRootViewController:vc];
        else admob::Interstitial::onPlatformEvent(5, "no ad ready");
    });
}
