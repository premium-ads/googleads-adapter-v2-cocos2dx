#include "AppOpen.h"
#include "AdMobBridgeCpp.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/jni/JniHelper.h"
    static const char* kJNI = "net/premiumads/cocos2dx/admob/AppOpenJNI";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extern "C" void admob_ios_appOpen_load(const char* adUnitId, int orientation);
    extern "C" bool admob_ios_appOpen_isReady();
    extern "C" void admob_ios_appOpen_show();
#endif

namespace admob {

AppOpen::Listener AppOpen::s_listener;
void AppOpen::setListener(Listener cb) { s_listener = std::move(cb); }

void AppOpen::load(const std::string& adUnitId, AppOpenOrientation orientation) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "load", "(Ljava/lang/String;I)V")) {
        jstring jId = info.env->NewStringUTF(adUnitId.c_str());
        info.env->CallStaticVoidMethod(info.classID, info.methodID, jId, (jint) orientation);
        info.env->DeleteLocalRef(jId); info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_appOpen_load(adUnitId.c_str(), (int) orientation);
#endif
}

bool AppOpen::isReady() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "isReady", "()Z")) {
        jboolean r = info.env->CallStaticBooleanMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
        return r == JNI_TRUE;
    }
    return false;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return admob_ios_appOpen_isReady();
#else
    return false;
#endif
}

void AppOpen::show() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "show", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_appOpen_show();
#endif
}

void AppOpen::onPlatformEvent(int code, const std::string& msg) {
    if (!s_listener) return;
    auto e = static_cast<AppOpenEvent>(code);
    auto m = msg; auto l = s_listener;
    dispatchOnCocosThread([l, e, m]() { l(e, m); });
}

}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_net_premiumads_cocos2dx_admob_AppOpenJNI_nativeOnEvent(
        JNIEnv* env, jclass, jint code, jstring msg) {
    const char* c = msg ? env->GetStringUTFChars(msg, nullptr) : "";
    admob::AppOpen::onPlatformEvent((int)code, std::string(c ? c : ""));
    if (msg) env->ReleaseStringUTFChars(msg, c);
}
#endif
