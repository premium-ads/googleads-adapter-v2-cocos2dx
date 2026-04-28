#import <UIKit/UIKit.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import "../../../Classes/AdMobBridge/NativeAd.h"

static GADNativeAd *sAd = nil;
static GADNativeAdView *sView = nil;
static GADAdLoader *sLoader = nil;

@interface PACNativeDelegate : NSObject <GADNativeAdLoaderDelegate, GADNativeAdDelegate>
@end
@implementation PACNativeDelegate
- (void)adLoader:(GADAdLoader *)adLoader didReceiveNativeAd:(GADNativeAd *)nativeAd {
    [sAd release];
    sAd = [nativeAd retain];   // MRC retain
    sAd.delegate = self;
    admob::NativeAd::onPlatformEvent(0, "");
}
- (void)adLoader:(GADAdLoader *)adLoader didFailToReceiveAdWithError:(NSError *)err {
    [sAd release]; sAd = nil;
    admob::NativeAd::onPlatformEvent(1, err.localizedDescription.UTF8String ?: "");
}
- (void)nativeAdDidRecordImpression:(GADNativeAd *)nativeAd { admob::NativeAd::onPlatformEvent(2, ""); }
- (void)nativeAdDidRecordClick:(GADNativeAd *)nativeAd      { admob::NativeAd::onPlatformEvent(3, ""); }
@end
static PACNativeDelegate *sDelegate = nil;

extern "C" void admob_ios_native_load(const char* adUnitId) {
    UIViewController *vc = UIApplication.sharedApplication.delegate.window.rootViewController;
    if (!sDelegate) sDelegate = [PACNativeDelegate new];
    sLoader = [[GADAdLoader alloc] initWithAdUnitID:[NSString stringWithUTF8String:adUnitId]
                                  rootViewController:vc
                                             adTypes:@[GADAdLoaderAdTypeNative]
                                             options:nil];
    sLoader.delegate = (id<GADAdLoaderDelegate>) sDelegate;
    [sLoader loadRequest:[GADRequest request]];
}

extern "C" bool admob_ios_native_isReady() { return sAd != nil; }

extern "C" void admob_ios_native_bind(float x, float y, float w, float h, float designH) {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (!sAd) return;
        if (sView) { [sView removeFromSuperview]; [sView release]; sView = nil; }

        UIView *parent = UIApplication.sharedApplication.delegate.window.rootViewController.view;
        CGFloat scale = parent.bounds.size.height / designH;
        CGFloat wPt = w * scale;
        CGFloat hPt = h * scale;
        CGFloat leftPt = x * scale;
        CGFloat topPt = parent.bounds.size.height - (y + h) * scale;

        sView = [[GADNativeAdView alloc] initWithFrame:CGRectMake(leftPt, topPt, wPt, hPt)];
        sView.backgroundColor = UIColor.whiteColor;
        sView.layer.borderColor = [UIColor colorWithWhite:0.85 alpha:1.0].CGColor;
        sView.layer.borderWidth = 1.0;
        sView.layer.cornerRadius = 6.0;

        // Layout: headline (24pt) → mediaView fills middle → body+CTA bottom (44pt).
        // No MAX guard: caller must pass hPt large enough that mediaH stays ≥120pt
        // (AdMob policy). Otherwise mediaH could push asset frames outside sView and
        // the AdMob validator flags "advertiser assets outside native ad view".
        CGFloat pad = 8;
        CGFloat headlineH = 24;
        CGFloat bottomH = 44;
        CGFloat mediaTop = pad + headlineH + 4;
        CGFloat mediaH = hPt - mediaTop - bottomH - pad;
        if (mediaH < 0) mediaH = 0;

        UILabel *headline = [[UILabel alloc] initWithFrame:CGRectMake(pad, pad, wPt - 2*pad, headlineH)];
        headline.font = [UIFont boldSystemFontOfSize:16];
        headline.text = sAd.headline;
        sView.headlineView = headline;
        [sView addSubview:headline];

        GADMediaView *media = [[GADMediaView alloc] initWithFrame:CGRectMake(pad, mediaTop, wPt - 2*pad, mediaH)];
        media.contentMode = UIViewContentModeScaleAspectFill;
        media.clipsToBounds = YES;
        sView.mediaView = media;
        [sView addSubview:media];

        CGFloat bodyTop = mediaTop + mediaH + 4;
        UILabel *body = [[UILabel alloc] initWithFrame:CGRectMake(pad, bodyTop, wPt - 100 - pad, 20)];
        body.font = [UIFont systemFontOfSize:13];
        body.numberOfLines = 1;
        body.text = sAd.body;
        sView.bodyView = body;
        [sView addSubview:body];

        UIButton *cta = [UIButton buttonWithType:UIButtonTypeSystem];
        cta.frame = CGRectMake(wPt - 92, bodyTop, 84, 28);
        cta.titleLabel.font = [UIFont boldSystemFontOfSize:13];
        [cta setTitle:sAd.callToAction forState:UIControlStateNormal];
        cta.backgroundColor = [UIColor colorWithRed:0.08 green:0.39 blue:0.86 alpha:1.0];
        [cta setTitleColor:UIColor.whiteColor forState:UIControlStateNormal];
        cta.layer.cornerRadius = 4.0;
        sView.callToActionView = cta;
        [sView addSubview:cta];

        sView.nativeAd = sAd;
        [parent addSubview:sView];
    });
}

extern "C" void admob_ios_native_destroy() {
    dispatch_async(dispatch_get_main_queue(), ^{
        [sView removeFromSuperview];
        [sView release]; sView = nil;
        [sAd release]; sAd = nil;
        admob::NativeAd::onPlatformEvent(4, "");
    });
}
