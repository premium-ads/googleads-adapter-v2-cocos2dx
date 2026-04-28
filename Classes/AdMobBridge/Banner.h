#ifndef ADMOB_BANNER_H
#define ADMOB_BANNER_H

#include <functional>
#include <string>

namespace admob {

enum class BannerEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked, Opened, Closed
};

enum class BannerPosition { Top = 0, Bottom = 1 };

// Maps to AdSize.BANNER (320x50), AdSize.MEDIUM_RECTANGLE (300x250),
// AdSize.LEADERBOARD (728x90), AdSize.getCurrentOrientationAnchoredAdaptiveBannerAdSize(...) on Android;
// GADAdSizeBanner, GADAdSizeMediumRectangle, GADAdSizeLeaderboard, GADCurrentOrientationAnchoredAdaptiveBannerAdSizeWithWidth(...) on iOS.
enum class BannerSize {
    Banner_320x50 = 0,
    MediumRectangle_300x250 = 1,
    Leaderboard_728x90 = 2,
    Adaptive = 3
};

class Banner {
public:
    using Listener = std::function<void(BannerEvent, const std::string& msg)>;

    static void create(const std::string& adUnitId, BannerSize size, BannerPosition position);
    static void show();
    static void hide();
    static void destroy();
    static void setListener(Listener cb);

    static void onPlatformEvent(int eventCode, const std::string& msg);

private:
    static Listener s_listener;
};

}  // namespace admob

#endif
