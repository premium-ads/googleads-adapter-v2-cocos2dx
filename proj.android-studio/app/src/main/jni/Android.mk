LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../../../cocos2d/cocos/audio/include)

LOCAL_MODULE := cocos2dcpp

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../../../../Classes/AppDelegate.cpp \
                   ../../../../../Classes/HelloWorldScene.cpp \
                   ../../../../../Classes/PremiumAdsAdapter/PremiumAdsAdapter.cpp \
                   ../../../../../Classes/AdMobBridge/Banner.cpp

LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdMobBridge/Interstitial.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdMobBridge/Rewarded.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdMobBridge/RewardedInterstitial.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdMobBridge/AppOpen.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdMobBridge/NativeAd.cpp
LOCAL_SRC_FILES += $(LOCAL_PATH)/../../../../../Classes/AdsLogBuf.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../Classes \
                    $(LOCAL_PATH)/../../../../../Classes/PremiumAdsAdapter \
                    $(LOCAL_PATH)/../../../../../Classes/AdMobBridge

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
