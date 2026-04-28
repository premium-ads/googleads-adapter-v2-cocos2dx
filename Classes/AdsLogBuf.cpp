#include "AdsLogBuf.h"

extern "C" void ads_demo_pushInitLog(const char* msg) {
    if (msg) adslog::push(std::string(msg));
}

#if (defined(__ANDROID__))
#include <jni.h>
extern "C" JNIEXPORT void JNICALL
Java_net_premiumads_cocos2dx_PremiumAdsAdapterBridge_nativePushInitLog(
        JNIEnv* env, jclass /*clazz*/, jstring jmsg) {
    if (!jmsg) return;
    const char* c = env->GetStringUTFChars(jmsg, nullptr);
    if (c) adslog::push(std::string(c));
    env->ReleaseStringUTFChars(jmsg, c);
}
#endif
