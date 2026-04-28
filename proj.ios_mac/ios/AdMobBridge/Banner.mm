#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import "../../../Classes/AdMobBridge/Banner.h"

static GADBannerView *sBanner = nil;

static UIViewController *rootVC() {
    UIWindow *w = UIApplication.sharedApplication.delegate.window;
    return w.rootViewController;
}

static GADAdSize gadSizeFor(int ordinal) {
    switch (ordinal) {
        case 1: return GADAdSizeMediumRectangle;
        case 2: return GADAdSizeLeaderboard;
        case 3: {
            CGFloat w = UIScreen.mainScreen.bounds.size.width;
            return GADCurrentOrientationAnchoredAdaptiveBannerAdSizeWithWidth(w);
        }
        default: return GADAdSizeBanner;
    }
}

@interface PACBannerDelegate : NSObject <GADBannerViewDelegate>
@end

@implementation PACBannerDelegate
- (void)bannerViewDidReceiveAd:(GADBannerView *)view {
    admob::Banner::onPlatformEvent(0, "");
}
- (void)bannerView:(GADBannerView *)view didFailToReceiveAdWithError:(NSError *)err {
    admob::Banner::onPlatformEvent(1, err.localizedDescription.UTF8String ?: "");
}
- (void)bannerViewDidRecordImpression:(GADBannerView *)view {
    admob::Banner::onPlatformEvent(2, "");
}
- (void)bannerViewDidRecordClick:(GADBannerView *)view {
    admob::Banner::onPlatformEvent(3, "");
}
- (void)bannerViewWillPresentScreen:(GADBannerView *)view {
    admob::Banner::onPlatformEvent(4, "");
}
- (void)bannerViewDidDismissScreen:(GADBannerView *)view {
    admob::Banner::onPlatformEvent(5, "");
}
@end

static PACBannerDelegate *sDelegate = nil;

extern "C" void admob_ios_banner_create(const char* adUnitId, int size, int position) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (sBanner) { [sBanner removeFromSuperview]; [sBanner release]; sBanner = nil; }
        sBanner = [[GADBannerView alloc] initWithAdSize:gadSizeFor(size)];
        sBanner.adUnitID = [NSString stringWithUTF8String:adUnitId];
        sBanner.rootViewController = rootVC();
        if (!sDelegate) sDelegate = [PACBannerDelegate new];
        sBanner.delegate = sDelegate;

        UIView *parent = rootVC().view;
        sBanner.translatesAutoresizingMaskIntoConstraints = NO;
        [parent addSubview:sBanner];
        if (position == 0) {
            [sBanner.topAnchor constraintEqualToAnchor:parent.safeAreaLayoutGuide.topAnchor].active = YES;
        } else {
            [sBanner.bottomAnchor constraintEqualToAnchor:parent.safeAreaLayoutGuide.bottomAnchor].active = YES;
        }
        [sBanner.centerXAnchor constraintEqualToAnchor:parent.centerXAnchor].active = YES;

        [sBanner loadRequest:[GADRequest request]];
    });
}

extern "C" void admob_ios_banner_show() {
    dispatch_async(dispatch_get_main_queue(), ^{ sBanner.hidden = NO; });
}

extern "C" void admob_ios_banner_hide() {
    dispatch_async(dispatch_get_main_queue(), ^{ sBanner.hidden = YES; });
}

extern "C" void admob_ios_banner_destroy() {
    dispatch_async(dispatch_get_main_queue(), ^{
        [sBanner removeFromSuperview];
        [sBanner release]; sBanner = nil;
    });
}
