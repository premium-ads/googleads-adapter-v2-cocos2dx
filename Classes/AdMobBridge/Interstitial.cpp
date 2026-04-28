#include "Interstitial.h"
#include "AdMobBridgeCpp.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/jni/JniHelper.h"
    static const char* kJNI = "net/premiumads/cocos2dx/admob/InterstitialJNI";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extern "C" void admob_ios_interstitial_load(const char* adUnitId);
    extern "C" bool admob_ios_interstitial_isReady();
    extern "C" void admob_ios_interstitial_show();
#endif

namespace admob {

Interstitial::Listener Interstitial::s_listener;

void Interstitial::setListener(Listener cb) { s_listener = std::move(cb); }

void Interstitial::load(const std::string& adUnitId) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "load", "(Ljava/lang/String;)V")) {
        jstring jId = info.env->NewStringUTF(adUnitId.c_str());
        info.env->CallStaticVoidMethod(info.classID, info.methodID, jId);
        info.env->DeleteLocalRef(jId);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_interstitial_load(adUnitId.c_str());
#endif
}

bool Interstitial::isReady() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "isReady", "()Z")) {
        jboolean r = info.env->CallStaticBooleanMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
        return r == JNI_TRUE;
    }
    return false;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return admob_ios_interstitial_isReady();
#else
    return false;
#endif
}

void Interstitial::show() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "show", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_interstitial_show();
#endif
}

void Interstitial::onPlatformEvent(int code, const std::string& msg) {
    if (!s_listener) return;
    auto e = static_cast<InterstitialEvent>(code);
    auto m = msg;
    auto l = s_listener;
    dispatchOnCocosThread([l, e, m]() { l(e, m); });
}

}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_net_premiumads_cocos2dx_admob_InterstitialJNI_nativeOnEvent(
        JNIEnv* env, jclass, jint code, jstring msg) {
    const char* c = msg ? env->GetStringUTFChars(msg, nullptr) : "";
    admob::Interstitial::onPlatformEvent((int)code, std::string(c ? c : ""));
    if (msg) env->ReleaseStringUTFChars(msg, c);
}
#endif
