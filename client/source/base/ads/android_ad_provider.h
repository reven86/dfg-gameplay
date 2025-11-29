#pragma once

#ifdef __ANDROID__

#include "ad_provider.h"
#include <jni.h>
#include <string>

class AndroidAdProvider : public AdProvider {
protected:
    JavaVM* jvm;
    jobject javaProvider;
    jclass javaClass;
    std::string providerName;

    // Method IDs cached for performance
    jmethodID initializeMethod;
    jmethodID loadRewardedAdMethod;
    jmethodID loadInterstitialAdMethod;
    jmethodID showRewardedAdMethod;
    jmethodID showInterstitialAdMethod;
    jmethodID showBannerAdMethod;
    jmethodID hideBannerAdMethod;
    jmethodID isRewardedAdLoadedMethod;
    jmethodID isInterstitialAdLoadedMethod;
    jmethodID getProviderNameMethod;

public:
    virtual ~AndroidAdProvider();

    static AndroidAdProvider* create(const std::string& name);

    // AdProvider implementation
    void initialize() override;
    void loadRewardedAd() override;
    void loadInterstitialAd() override;
    void showRewardedAd() override;
    void showInterstitialAd() override;
    void showBannerAd() override;
    void hideBannerAd() override;
    bool isRewardedAdLoaded() const override;
    bool isInterstitialAdLoaded() const override;
    std::string getProviderName() const override;

protected:
    AndroidAdProvider() {};

    JNIEnv* getJNIEnv() const;
    void callVoidMethod(jmethodID method) const;
    bool callBooleanMethod(jmethodID method) const;
    std::string callStringMethod(jmethodID method) const;
};


#endif