#ifndef ADMOB_REWARDED_INTERSTITIAL_H
#define ADMOB_REWARDED_INTERSTITIAL_H

#include <functional>
#include <string>

namespace admob {

enum class RewardedInterstitialEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked,
    Showed, FailedToShow, Dismissed, UserEarnedReward
};

class RewardedInterstitial {
public:
    using Listener = std::function<void(RewardedInterstitialEvent, const std::string& msg)>;

    static void load(const std::string& adUnitId);
    static bool isReady();
    static void show();
    static void setListener(Listener cb);

    static void onPlatformEvent(int code, const std::string& msg);

private:
    static Listener s_listener;
};

}

#endif
