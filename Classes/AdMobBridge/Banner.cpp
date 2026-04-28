#include "Banner.h"
#include "AdMobBridgeCpp.h"
#include "platform/CCPlatformConfig.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "platform/android/jni/JniHelper.h"
    static const char* kBannerJNI = "net/premiumads/cocos2dx/admob/BannerJNI";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    extern "C" void admob_ios_banner_create(const char* adUnitId, int size, int position);
    extern "C" void admob_ios_banner_show();
    extern "C" void admob_ios_banner_hide();
    extern "C" void admob_ios_banner_destroy();
#endif

namespace admob {

Banner::Listener Banner::s_listener;

void Banner::setListener(Listener cb) { s_listener = std::move(cb); }

void Banner::create(const std::string& adUnitId, BannerSize size, BannerPosition position) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kBannerJNI, "create", "(Ljava/lang/String;II)V")) {
        jstring jId = info.env->NewStringUTF(adUnitId.c_str());
        info.env->CallStaticVoidMethod(info.classID, info.methodID, jId, (jint) size, (jint) position);
        info.env->DeleteLocalRef(jId);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_banner_create(adUnitId.c_str(), (int) size, (int) position);
#endif
}

void Banner::show() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kBannerJNI, "show", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_banner_show();
#endif
}

void Banner::hide() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kBannerJNI, "hide", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_banner_hide();
#endif
}

void Banner::destroy() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    cocos2d::JniMethodInfo info;
    if (cocos2d::JniHelper::getStaticMethodInfo(info, kBannerJNI, "destroy", "()V")) {
        info.env->CallStaticVoidMethod(info.classID, info.methodID);
        info.env->DeleteLocalRef(info.classID);
    }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    admob_ios_banner_destroy();
#endif
}

void Banner::onPlatformEvent(int eventCode, const std::string& msg) {
    if (!s_listener) return;
    auto code = static_cast<BannerEvent>(eventCode);
    auto msgCopy = msg;
    auto listenerCopy = s_listener;
    dispatchOnCocosThread([listenerCopy, code, msgCopy]() {
        listenerCopy(code, msgCopy);
    });
}

}  // namespace admob

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C" JNIEXPORT void JNICALL
Java_net_premiumads_cocos2dx_admob_BannerJNI_nativeOnEvent(
        JNIEnv* env, jclass /*clazz*/, jint code, jstring msg) {
    const char* cstr = msg ? env->GetStringUTFChars(msg, nullptr) : "";
    admob::Banner::onPlatformEvent((int) code, std::string(cstr ? cstr : ""));
    if (msg) env->ReleaseStringUTFChars(msg, cstr);
}
#endif
