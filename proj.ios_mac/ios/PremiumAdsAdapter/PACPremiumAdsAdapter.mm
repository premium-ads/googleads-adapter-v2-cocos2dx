#import <Foundation/Foundation.h>
#import <GoogleMobileAds/GoogleMobileAds.h>
#import <PremiumAdsGoogleAdapter/PremiumAdsGoogleAdapter-Swift.h>
#import "PACPremiumAdsAdapter.h"

void premiumads_ios_setDebug(bool enabled) {
    [PremiumAdsAdapter setDebug:enabled];
}
