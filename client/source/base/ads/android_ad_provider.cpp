#include "pch.h"
#include "android_ad_provider.h"
#include <iostream>

#ifdef __ANDROID__
#include <android_native_app_glue.h>

extern struct android_app* __state;

AndroidAdProvider* AndroidAdProvider::create(const std::string& name)
{
    android_app* app = __state;
    JNIEnv* env = app->activity->env;
    JavaVM* vm = app->activity->vm;

    vm->AttachCurrentThread(&env, NULL);

    // Find the AdMobProvider class
    std::string className = std::string("com.dreamfarmgames.util.") + name + "Provider";
    
    // doesn't work, see https://stackoverflow.com/questions/14586821/android-flurry-integration-with-ndk-app
    //jclass adMobClass = env->FindClass(className.c_str());
    jobject nativeActivity = app->activity->clazz;
    jclass acl = env->GetObjectClass(nativeActivity);
    jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject cls = env->CallObjectMethod(nativeActivity, getClassLoader);
    jclass classLoader = env->FindClass("java/lang/ClassLoader");
    jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    jstring strClassName = env->NewStringUTF(className.c_str());
    jclass adMobClass = (jclass)(env->CallObjectMethod(cls, findClass, strClassName));
    env->DeleteLocalRef(strClassName);

    if (!adMobClass) {
        env->ExceptionClear();
        GP_WARN("Can't find class %s", className.c_str());
        vm->DetachCurrentThread();
        return nullptr;
    }

    // Get the constructor method ID
    jmethodID constructor = env->GetMethodID(adMobClass, "<init>", "(Landroid/app/Activity;)V");
    if (!constructor) {
        vm->DetachCurrentThread();
        return nullptr;
    }

    // Create the Java AdMobProvider instance
    jobject provider = env->NewObject(adMobClass, constructor, nativeActivity);
    if (!provider) {
        vm->DetachCurrentThread();
        return nullptr;
    }

    AndroidAdProvider* res = new AndroidAdProvider();

    res->jvm = vm;

    // Create a global reference so it doesn't get garbage collected
    res->javaProvider = env->NewGlobalRef(provider);
    res->javaClass = (jclass)env->NewGlobalRef(adMobClass);
    res->providerName = name;

    // Clean up local references
    env->DeleteLocalRef(provider);

    // Cache method IDs
    res->initializeMethod = env->GetMethodID(res->javaClass, "initialize", "(Ljava/lang/String;Ljava/lang/String;)V");
    res->loadRewardedAdMethod = env->GetMethodID(res->javaClass, "loadRewardedAd", "()V");
    res->loadInterstitialAdMethod = env->GetMethodID(res->javaClass, "loadInterstitialAd", "()V");
    res->showRewardedAdMethod = env->GetMethodID(res->javaClass, "showRewardedAd", "()V");
    res->showInterstitialAdMethod = env->GetMethodID(res->javaClass, "showInterstitialAd", "()V");
    res->isRewardedAdLoadedMethod = env->GetMethodID(res->javaClass, "isRewardedAdLoaded", "()Z");
    res->isInterstitialAdLoadedMethod = env->GetMethodID(res->javaClass, "isInterstitialAdLoaded", "()Z");

    vm->DetachCurrentThread();

    return res;
}

AndroidAdProvider::~AndroidAdProvider() {
    JNIEnv* env = getJNIEnv();
    if (javaProvider) {
        env->DeleteGlobalRef(javaProvider);
    }
    if (javaClass) {
        env->DeleteGlobalRef(javaClass);
    }
}

JNIEnv* AndroidAdProvider::getJNIEnv() const {
    JNIEnv* env;
    jint result = jvm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (result == JNI_EDETACHED) {
        // Attach current thread to JVM
        jvm->AttachCurrentThread(&env, nullptr);
    }
    return env;
}

void AndroidAdProvider::callVoidMethod(jmethodID method) const {
    JNIEnv* env = getJNIEnv();
    env->CallVoidMethod(javaProvider, method);
}

bool AndroidAdProvider::callBooleanMethod(jmethodID method) const {
    JNIEnv* env = getJNIEnv();
    return env->CallBooleanMethod(javaProvider, method);
}

std::string AndroidAdProvider::callStringMethod(jmethodID method) const {
    JNIEnv* env = getJNIEnv();
    jstring jstr = (jstring)env->CallObjectMethod(javaProvider, method);
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    env->DeleteLocalRef(jstr);
    return result;
}

void AndroidAdProvider::initialize(const std::unordered_map<std::string, std::string>& properties) {
    JNIEnv* env = getJNIEnv();

    auto it_interstitialAdId = properties.find("interstitialAdId");
    std::string interstitialAdId = it_interstitialAdId == properties.end() ? "" : it_interstitialAdId->second;

    auto it_rewardedAdId = properties.find("rewardedAdId");
    std::string rewardedAdId = it_rewardedAdId == properties.end() ? "" : it_rewardedAdId->second;

    jstring strIntersitialAdId = env->NewStringUTF(interstitialAdId.c_str());
    jstring strRewardedAdId = env->NewStringUTF(rewardedAdId.c_str());
    env->CallVoidMethod(javaProvider, initializeMethod, strIntersitialAdId, strRewardedAdId);
    env->DeleteLocalRef(strIntersitialAdId);
    env->DeleteLocalRef(strRewardedAdId);
}

void AndroidAdProvider::loadRewardedAd() {
    callVoidMethod(loadRewardedAdMethod);
}

void AndroidAdProvider::loadInterstitialAd() {
    callVoidMethod(loadInterstitialAdMethod);
}

void AndroidAdProvider::showRewardedAd() {
    callVoidMethod(showRewardedAdMethod);
}

void AndroidAdProvider::showInterstitialAd() {
    callVoidMethod(showInterstitialAdMethod);
}

bool AndroidAdProvider::isRewardedAdLoaded() const {
    return callBooleanMethod(isRewardedAdLoadedMethod);
}

bool AndroidAdProvider::isInterstitialAdLoaded() const {
    return callBooleanMethod(isInterstitialAdLoadedMethod);
}

std::string AndroidAdProvider::getProviderName() const {
    return providerName;
}

#endif