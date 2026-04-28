#include "HelloWorldScene.h"
#include "AdMobBridge/Banner.h"
#include "AdMobBridge/Interstitial.h"
#include "AdMobBridge/Rewarded.h"
#include "AdMobBridge/RewardedInterstitial.h"
#include "AdMobBridge/AppOpen.h"
#include "AdMobBridge/NativeAd.h"
#include "PremiumAdsAdapter/PremiumAdsAdapter.h"
#include "AdsLogBuf.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

USING_NS_CC;

namespace {
// TODO: replace with your own AdMob ad unit IDs before shipping.
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    const char* kBannerId    = "<your admob ad unit>";
    const char* kInterId     = "<your admob ad unit>";
    const char* kRewardedId  = "<your admob ad unit>";
    const char* kRwInterId   = "<your admob ad unit>";
    const char* kAppOpenId   = "<your admob ad unit>";
    const char* kNativeId    = "<your admob ad unit>";
#else
    const char* kBannerId    = "<your admob ad unit>";
    const char* kInterId     = "<your admob ad unit>";
    const char* kRewardedId  = "<your admob ad unit>";
    const char* kRwInterId   = "<your admob ad unit>";
    const char* kAppOpenId   = "<your admob ad unit>";
    const char* kNativeId    = "<your admob ad unit>";
#endif

const char* bannerEventName(int e) {
    switch (e) {
        case 0: return "Banner loaded";
        case 1: return "Banner failed to load";
        case 2: return "Banner impression";
        case 3: return "Banner clicked";
        case 4: return "Banner opened";
        case 5: return "Banner closed";
        default: return "Banner event";
    }
}
const char* interstitialEventName(int e) {
    switch (e) {
        case 0: return "Interstitial loaded";
        case 1: return "Interstitial failed to load";
        case 2: return "Interstitial impression";
        case 3: return "Interstitial clicked";
        case 4: return "Interstitial showed";
        case 5: return "Interstitial failed to show";
        case 6: return "Interstitial dismissed";
        default: return "Interstitial event";
    }
}
const char* rewardedEventName(int e) {
    switch (e) {
        case 0: return "Rewarded loaded";
        case 1: return "Rewarded failed to load";
        case 2: return "Rewarded impression";
        case 3: return "Rewarded clicked";
        case 4: return "Rewarded showed";
        case 5: return "Rewarded failed to show";
        case 6: return "Rewarded dismissed";
        case 7: return "Rewarded earned";
        default: return "Rewarded event";
    }
}
const char* rwInterEventName(int e) {
    switch (e) {
        case 0: return "Rewarded Interstitial loaded";
        case 1: return "Rewarded Interstitial failed to load";
        case 2: return "Rewarded Interstitial impression";
        case 3: return "Rewarded Interstitial clicked";
        case 4: return "Rewarded Interstitial showed";
        case 5: return "Rewarded Interstitial failed to show";
        case 6: return "Rewarded Interstitial dismissed";
        case 7: return "Rewarded Interstitial earned";
        default: return "Rewarded Interstitial event";
    }
}
const char* appOpenEventName(int e) {
    switch (e) {
        case 0: return "App Open loaded";
        case 1: return "App Open failed to load";
        case 2: return "App Open impression";
        case 3: return "App Open clicked";
        case 4: return "App Open showed";
        case 5: return "App Open failed to show";
        case 6: return "App Open dismissed";
        default: return "App Open event";
    }
}
const char* nativeEventName(int e) {
    switch (e) {
        case 0: return "Native loaded";
        case 1: return "Native failed to load";
        case 2: return "Native impression";
        case 3: return "Native clicked";
        case 4: return "Native closed";
        default: return "Native event";
    }
}

}

Scene* HelloWorld::createScene() { return HelloWorld::create(); }

std::string HelloWorld::timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);
    std::ostringstream os;
    os << "[" << std::setw(2) << std::setfill('0') << tm.tm_hour
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_min
       << ":" << std::setw(2) << std::setfill('0') << tm.tm_sec << "] ";
    return os.str();
}

void HelloWorld::appendLog(const std::string& s) {
    std::string line = timestamp() + s;
    _logLines.push_back(line);
    while (_logLines.size() > kMaxLogLines) _logLines.pop_front();
    if (_logLabel) {
        std::string joined;
        for (auto& l : _logLines) { joined += l; joined += "\n"; }
        _logLabel->setString(joined);
    }
    log("%s", line.c_str());
}

bool HelloWorld::init() {
    if (!Scene::init()) return false;

    Size visible = Director::getInstance()->getVisibleSize();
    Vec2 origin  = Director::getInstance()->getVisibleOrigin();
    float cx = origin.x + visible.width / 2;

    auto bg = LayerColor::create(Color4B(255, 255, 255, 255));
    addChild(bg, -1);

    auto title = Label::createWithSystemFont("PremiumAds Cocos Sample", "Helvetica-Bold", 22);
    title->setTextColor(Color4B(0, 0, 0, 255));
    title->setPosition(cx, origin.y + visible.height - 40);
    addChild(title);

    auto makeBtn = [&](const char* label, float y, const ccMenuCallback& cb) {
        auto item = MenuItemFont::create(label, cb);
        item->setFontSizeObj(18);
        item->setColor(Color3B(20, 100, 220));
        item->setPosition(Vec2(cx, y));
        return item;
    };

    float topY = origin.y + visible.height - 100;
    float spacing = 40;

    auto* menu = Menu::create(
        makeBtn("Load Banner",                topY - 0 * spacing, CC_CALLBACK_1(HelloWorld::onTapBanner, this)),
        makeBtn("Load Interstitial",          topY - 1 * spacing, CC_CALLBACK_1(HelloWorld::onTapInterstitial, this)),
        makeBtn("Load Rewarded",              topY - 2 * spacing, CC_CALLBACK_1(HelloWorld::onTapRewarded, this)),
        makeBtn("Load Rewarded Interstitial", topY - 3 * spacing, CC_CALLBACK_1(HelloWorld::onTapRewardedInterstitial, this)),
        makeBtn("Load Native",                topY - 4 * spacing, CC_CALLBACK_1(HelloWorld::onTapNative, this)),
        makeBtn("Load App Open",              topY - 5 * spacing, CC_CALLBACK_1(HelloWorld::onTapAppOpen, this)),
        nullptr);
    menu->setPosition(Vec2::ZERO);
    addChild(menu);

    // Log top sits just under the last button (y=340) so the 8-line log column
    // ends at design-y ~224 — leaves a small gap above the Native overlay (top
    // edge at design-y 220).
    float logTopY = topY - 5 * spacing - 20;
    _logLabel = Label::createWithSystemFont("Logs will appear here...\n", "Courier", 10);
    _logLabel->setTextColor(Color4B(0, 0, 0, 255));
    _logLabel->setAnchorPoint(Vec2(0, 1));
    _logLabel->setPosition(origin.x + 12, logTopY);
    _logLabel->setMaxLineWidth(visible.width - 24);
    addChild(_logLabel);

    admob::Banner::setListener([this](admob::BannerEvent e, const std::string& m){
        std::string s = bannerEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
    });
    admob::Interstitial::setListener([this](admob::InterstitialEvent e, const std::string& m){
        std::string s = interstitialEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
        if (e == admob::InterstitialEvent::Loaded && _interPendingShow) {
            _interPendingShow = false;
            appendLog("Showing interstitial...");
            admob::Interstitial::show();
        }
    });
    admob::Rewarded::setListener([this](admob::RewardedEvent e, const std::string& m){
        std::string s = rewardedEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
        if (e == admob::RewardedEvent::Loaded && _rewardedPendingShow) {
            _rewardedPendingShow = false;
            appendLog("Showing rewarded...");
            admob::Rewarded::show();
        }
    });
    admob::RewardedInterstitial::setListener([this](admob::RewardedInterstitialEvent e, const std::string& m){
        std::string s = rwInterEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
        if (e == admob::RewardedInterstitialEvent::Loaded && _rwInterPendingShow) {
            _rwInterPendingShow = false;
            appendLog("Showing rewarded interstitial...");
            admob::RewardedInterstitial::show();
        }
    });
    admob::AppOpen::setListener([this](admob::AppOpenEvent e, const std::string& m){
        std::string s = appOpenEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
        if (e == admob::AppOpenEvent::Loaded && _appOpenPendingShow) {
            _appOpenPendingShow = false;
            appendLog("Showing app open...");
            admob::AppOpen::show();
        }
    });
    admob::NativeAd::setListener([this](admob::NativeAdEvent e, const std::string& m){
        std::string s = nativeEventName((int)e);
        if (!m.empty()) s += ": " + m;
        appendLog(s);
        if (e == admob::NativeAdEvent::Loaded && _nativePendingBind) {
            _nativePendingBind = false;
            Size sz = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
            float h = 220;        // design-px — Native top ends at design-y 240, log (6 lines @ y320 → y248) clears it by 8px
            float w = sz.width - 40;
            float y = 20;          // anchor at bottom edge — does not overlap the button column
            appendLog("Binding native ad...");
            admob::NativeAd::bind(20, y, w, h);
        }
    });

    // Drain init-log buffer (filled by MainApplication.java / AppController.mm
    // when MobileAds finishes initialising), and keep draining periodically in
    // case the callback fires after scene loads.
    auto drainAndLog = [this](float){
        for (auto& l : adslog::drain()) appendLog(l);
    };
    drainAndLog(0);
    this->schedule(drainAndLog, 0.5f, "log_drain");

    return true;
}

void HelloWorld::onTapBanner(Ref*) {
    appendLog("Loading banner...");
    admob::Banner::create(kBannerId, admob::BannerSize::Banner_320x50, admob::BannerPosition::Bottom);
}
void HelloWorld::onTapInterstitial(Ref*) {
    if (admob::Interstitial::isReady()) {
        appendLog("Showing interstitial...");
        admob::Interstitial::show();
        return;
    }
    appendLog("Loading interstitial...");
    _interPendingShow = true;
    admob::Interstitial::load(kInterId);
}
void HelloWorld::onTapRewarded(Ref*) {
    if (admob::Rewarded::isReady()) {
        appendLog("Showing rewarded...");
        admob::Rewarded::show();
        return;
    }
    appendLog("Loading rewarded...");
    _rewardedPendingShow = true;
    admob::Rewarded::load(kRewardedId);
}
void HelloWorld::onTapRewardedInterstitial(Ref*) {
    if (admob::RewardedInterstitial::isReady()) {
        appendLog("Showing rewarded interstitial...");
        admob::RewardedInterstitial::show();
        return;
    }
    appendLog("Loading rewarded interstitial...");
    _rwInterPendingShow = true;
    admob::RewardedInterstitial::load(kRwInterId);
}
void HelloWorld::onTapAppOpen(Ref*) {
    if (admob::AppOpen::isReady()) {
        appendLog("Showing app open...");
        admob::AppOpen::show();
        return;
    }
    appendLog("Loading app open...");
    _appOpenPendingShow = true;
    admob::AppOpen::load(kAppOpenId, admob::AppOpenOrientation::Portrait);
}
void HelloWorld::onTapNative(Ref*) {
    if (admob::NativeAd::isReady()) {
        Size sz = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
        appendLog("Binding native ad...");
        admob::NativeAd::bind(sz.width/2 - 160, 80, 320, 200);
        return;
    }
    appendLog("Loading native...");
    _nativePendingBind = true;
    admob::NativeAd::load(kNativeId);
}
