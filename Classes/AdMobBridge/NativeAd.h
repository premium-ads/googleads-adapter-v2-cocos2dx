#ifndef ADMOB_NATIVE_AD_H
#define ADMOB_NATIVE_AD_H

#include <functional>
#include <string>

namespace admob {

enum class NativeAdEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked, Closed
};

class NativeAd {
public:
    using Listener = std::function<void(NativeAdEvent, const std::string& msg)>;

    static void load(const std::string& adUnitId);
    static bool isReady();
    /// Position is in Cocos2d-x design coords (origin bottom-left).
    /// Peer converts to physical pixels and to UIKit coords (top-left).
    static void bind(float xDesign, float yDesign, float wDesign, float hDesign);
    static void destroy();
    static void setListener(Listener cb);

    static void onPlatformEvent(int code, const std::string& msg);

private:
    static Listener s_listener;
};

}

#endif
