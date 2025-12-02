#pragma once

#include "ad_provider.h"

class IOSUnityAdsProvider : public AdProvider {
public:
    IOSUnityAdsProvider();
    ~IOSUnityAdsProvider();
    
    // AdProvider implementation
    void initialize(const std::unordered_map<std::string, std::string>& properties) override;
    void loadRewardedAd() override;
    void loadInterstitialAd() override;
    void showRewardedAd() override;
    void showInterstitialAd() override;
    bool isRewardedAdLoaded() const override;
    bool isInterstitialAdLoaded() const override;

    virtual std::string getProviderName() const override { return "UnityAds"; };
    
private:
    void* platformProvider;
    bool isInitialized;
};