#ifndef ADMOB_APP_OPEN_H
#define ADMOB_APP_OPEN_H

#include <functional>
#include <string>

namespace admob {

enum class AppOpenEvent {
    Loaded, FailedToLoad, ImpressionRecorded, Clicked,
    Showed, FailedToShow, Dismissed
};

enum class AppOpenOrientation { Portrait = 1, Landscape = 2 };

class AppOpen {
public:
    using Listener = std::function<void(AppOpenEvent, const std::string& msg)>;

    static void load(const std::string& adUnitId, AppOpenOrientation orientation);
    static bool isReady();
    static void show();
    static void setListener(Listener cb);

    static void onPlatformEvent(int code, const std::string& msg);

private:
    static Listener s_listener;
};

}

#endif
