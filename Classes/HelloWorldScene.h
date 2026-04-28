#ifndef HELLO_WORLD_SCENE_H
#define HELLO_WORLD_SCENE_H

#include "cocos2d.h"
#include <deque>
#include <string>

class HelloWorld : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    bool init() override;
    CREATE_FUNC(HelloWorld);

private:
    cocos2d::Label* _logLabel = nullptr;
    std::deque<std::string> _logLines;
    static constexpr size_t kMaxLogLines = 6;

    // Single-tap demo flow: tap a "Load X" button, on Loaded event auto-show.
    bool _interPendingShow = false;
    bool _rewardedPendingShow = false;
    bool _rwInterPendingShow = false;
    bool _appOpenPendingShow = false;
    bool _nativePendingBind = false;

    void appendLog(const std::string& s);
    static std::string timestamp();

    void onTapBanner(cocos2d::Ref*);
    void onTapInterstitial(cocos2d::Ref*);
    void onTapRewarded(cocos2d::Ref*);
    void onTapRewardedInterstitial(cocos2d::Ref*);
    void onTapAppOpen(cocos2d::Ref*);
    void onTapNative(cocos2d::Ref*);
};

#endif
