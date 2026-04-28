#ifndef ADMOB_REWARDED_H
#define ADMOB_REWARDED_H

#include <functional>
#include <string>

namespace admob {

enum class RewardedEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked,
    Showed, FailedToShow, Dismissed,
    UserEarnedReward    // msg formatted "type|amount"
};

class Rewarded {
public:
    using Listener = std::function<void(RewardedEvent, const std::string& msg)>;

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
