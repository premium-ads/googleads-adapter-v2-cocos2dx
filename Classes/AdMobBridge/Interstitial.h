#ifndef ADMOB_INTERSTITIAL_H
#define ADMOB_INTERSTITIAL_H

#include <functional>
#include <string>

namespace admob {

enum class InterstitialEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked,
    Showed, FailedToShow, Dismissed
};

class Interstitial {
public:
    using Listener = std::function<void(InterstitialEvent, const std::string& msg)>;

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
