#include "pch.h"

#ifdef __ANDROID__

#include <jni.h>
#include <string>
#include "services/ad_service.h"
#include "services/service_manager.h"

// Global reference to the AdManager
AdService * g_adManager = nullptr;

// Helper function to get AdManager instance
AdService* getAdManager() {
    if (!g_adManager) {
        g_adManager = ServiceManager::getInstance()->findService<AdService>();
    }
    return g_adManager;
}

static void onProviderInitializedNative(JNIEnv* env, jobject thiz,
    jstring provider_name,
    jboolean success,
    jstring message)
{
    const char* providerName = env->GetStringUTFChars(provider_name, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);

    GP_LOG("%s initialized: success: %s message: %s", providerName, success ? "true" : "false", msg);

    auto* manager = getAdManager();
    if (manager) {
        manager->onAdEvent("provider_initialized", success, msg);
    }

    env->ReleaseStringUTFChars(provider_name, providerName);
    env->ReleaseStringUTFChars(message, msg);
}

static void onAdLoadedNative(JNIEnv* env, jobject thiz,
    jstring provider_name,
    jstring ad_type,
    jboolean success,
    jstring message)
{
    const char* providerName = env->GetStringUTFChars(provider_name, nullptr);
    const char* adType = env->GetStringUTFChars(ad_type, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);

    GP_LOG("%s ad loaded - Type: %s success: %s message: %s", providerName, adType, success ? "true" : "false", msg);

    auto* manager = getAdManager();
    if (manager) {
        std::string eventType = std::string(adType) + "_loaded";
        manager->onAdEvent(eventType, success, msg);
    }

    env->ReleaseStringUTFChars(provider_name, providerName);
    env->ReleaseStringUTFChars(ad_type, adType);
    env->ReleaseStringUTFChars(message, msg);
}

static void onAdShownNative(JNIEnv* env, jobject thiz,
    jstring provider_name,
    jstring ad_type,
    jboolean success,
    jstring message)
{
    const char* providerName = env->GetStringUTFChars(provider_name, nullptr);
    const char* adType = env->GetStringUTFChars(ad_type, nullptr);
    const char* msg = env->GetStringUTFChars(message, nullptr);

    GP_LOG("%s ad shown - Type: %s success: %s message: %s", providerName, adType, success ? "true" : "false", msg);

    auto* manager = getAdManager();
    if (manager) {
        std::string eventType = std::string(adType) + "_shown";
        manager->onAdEvent(eventType, success, msg);
    }

    env->ReleaseStringUTFChars(provider_name, providerName);
    env->ReleaseStringUTFChars(ad_type, adType);
    env->ReleaseStringUTFChars(message, msg);
}

static void onRewardEarnedNative(JNIEnv* env, jobject thiz,
    jint amount,
    jstring type)
{
    const char* rewardType = env->GetStringUTFChars(type, nullptr);

    GP_LOG("reward earned - Amount: %d Type:%s", amount, rewardType);

    auto* manager = getAdManager();
    if (manager) {
        manager->onRewardEarned(amount, rewardType);
    }

    env->ReleaseStringUTFChars(type, rewardType);
}

#ifdef __cplusplus
extern "C" {
#endif

// Provider Initialization Callback
JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_AdMobProvider_onProviderInitializedNative(JNIEnv *env, jobject thiz, 
                                                                        jstring provider_name, 
                                                                        jboolean success, 
                                                                        jstring message) {
    onProviderInitializedNative(env, thiz, provider_name, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_UnityAdsProvider_onProviderInitializedNative(JNIEnv *env, jobject thiz, 
                                                                          jstring provider_name, 
                                                                          jboolean success, 
                                                                          jstring message) {
    onProviderInitializedNative(env, thiz, provider_name, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_YandexAdsProvider_onProviderInitializedNative(JNIEnv *env, jobject thiz, 
                                                                          jstring provider_name, 
                                                                          jboolean success, 
                                                                          jstring message) {
    onProviderInitializedNative(env, thiz, provider_name, success, message);
}

// Ad Loaded Callback
JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_AdMobProvider_onAdLoadedNative(JNIEnv *env, jobject thiz, 
                                                            jstring provider_name, 
                                                            jstring ad_type, 
                                                            jboolean success, 
                                                            jstring message) {
    onAdLoadedNative(env, thiz, provider_name, ad_type, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_UnityAdsProvider_onAdLoadedNative(JNIEnv *env, jobject thiz, 
                                                               jstring provider_name, 
                                                               jstring ad_type, 
                                                               jboolean success, 
                                                               jstring message) {
    onAdLoadedNative(env, thiz, provider_name, ad_type, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_YandexAdsProvider_onAdLoadedNative(JNIEnv *env, jobject thiz, 
                                                               jstring provider_name, 
                                                               jstring ad_type, 
                                                               jboolean success, 
                                                               jstring message) {
    onAdLoadedNative(env, thiz, provider_name, ad_type, success, message);
}

// Ad Shown Callback
JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_AdMobProvider_onAdShownNative(JNIEnv *env, jobject thiz, 
                                                           jstring provider_name, 
                                                           jstring ad_type, 
                                                           jboolean success, 
                                                           jstring message) {
    onAdShownNative(env, thiz, provider_name, ad_type, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_UnityAdsProvider_onAdShownNative(JNIEnv *env, jobject thiz, 
                                                              jstring provider_name, 
                                                              jstring ad_type, 
                                                              jboolean success, 
                                                              jstring message) {
    onAdShownNative(env, thiz, provider_name, ad_type, success, message);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_YandexAdsProvider_onAdShownNative(JNIEnv *env, jobject thiz, 
                                                              jstring provider_name, 
                                                              jstring ad_type, 
                                                              jboolean success, 
                                                              jstring message) {
    onAdShownNative(env, thiz, provider_name, ad_type, success, message);
}

// Reward Earned Callback
JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_AdMobProvider_onRewardEarnedNative(JNIEnv *env, jobject thiz, 
                                                                jint amount, 
                                                                jstring type) {
    onRewardEarnedNative(env, thiz, amount, type);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_UnityAdsProvider_onRewardEarnedNative(JNIEnv *env, jobject thiz, 
                                                                   jint amount, 
                                                                   jstring type) {
    onRewardEarnedNative(env, thiz, amount, type);
}

JNIEXPORT void JNICALL
Java_com_dreamfarmgames_util_YandexAdsProvider_onRewardEarnedNative(JNIEnv *env, jobject thiz, 
                                                                   jint amount, 
                                                                   jstring type) {
    onRewardEarnedNative(env, thiz, amount, type);
}

#ifdef __cplusplus
}
#endif

#endif // __ANDROID__