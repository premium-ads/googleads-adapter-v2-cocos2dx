#include "PremiumAdsAdapter.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/jni/JniHelper.h"
#endif

namespace premiumads {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
extern "C" void premiumads_ios_setDebug(bool enabled);
#endif

void PremiumAdsAdapter::setDebug(bool enabled) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info,
            "net/premiumads/cocos2dx/PremiumAdsAdapterBridge",
            "setDebug",
            "(Z)V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID, enabled);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    premiumads_ios_setDebug(enabled);
#endif
}

}  // namespace premiumads
