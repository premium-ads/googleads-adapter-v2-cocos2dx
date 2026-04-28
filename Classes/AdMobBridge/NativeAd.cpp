#include "NativeAd.h"
#include "AdMobBridgeCpp.h"
#include "platform/CCPlatformConfig.h"
#include "cocos2d.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/jni/JniHelper.h"
    static const char* kJNI = "net/premiumads/cocos2dx/admob/NativeAdJNI";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extern "C" void admob_ios_native_load(const char* adUnitId);
    extern "C" bool admob_ios_native_isReady();
    extern "C" void admob_ios_native_bind(float x, float y, float w, float h, float designH);
    extern "C" void admob_ios_native_destroy();
#endif

namespace admob {

NativeAd::Listener NativeAd::s_listener;
void NativeAd::setListener(Listener cb) { s_listener = std::move(cb); }

void NativeAd::load(const std::string& adUnitId) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "load", "(Ljava/lang/String;)V")) {
        jstring jId = info.env->NewStringUTF(adUnitId.c_str());
        info.env->CallStaticVoidMethod(info.classID, info.methodID, jId);
        info.env->DeleteLocalRef(jId); info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_native_load(adUnitId.c_str());
#endif
}

bool NativeAd::isReady() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "isReady", "()Z")) {
        jboolean r = info.env->CallStaticBooleanMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
        return r == JNI_TRUE;
    }
    return false;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return admob_ios_native_isReady();
#else
    return false;
#endif
}

void NativeAd::bind(float x, float y, float w, float h) {
    auto* dir = cocos2d::Director::getInstance();
    cocos2d::Size designSize = dir->getOpenGLView()->getDesignResolutionSize();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "bind", "(FFFFF)V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID,
            (jfloat) x, (jfloat) y, (jfloat) w, (jfloat) h, (jfloat) designSize.height);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_native_bind(x, y, w, h, designSize.height);
#endif
}

void NativeAd::destroy() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kJNI, "destroy", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_native_destroy();
#endif
}

void NativeAd::onPlatformEvent(int code, const std::string& msg) {
    if (!s_listener) return;
    auto e = static_cast<NativeAdEvent>(code);
    auto m = msg; auto l = s_listener;
    dispatchOnCocosThread([l, e, m]() { l(e, m); });
}

}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_net_premiumads_cocos2dx_admob_NativeAdJNI_nativeOnEvent(
        JNIEnv* env, jclass, jint code, jstring msg) {
    const char* c = msg ? env->GetStringUTFChars(msg, nullptr) : "";
    admob::NativeAd::onPlatformEvent((int)code, std::string(c ? c : ""));
    if (msg) env->ReleaseStringUTFChars(msg, c);
}
#endif
