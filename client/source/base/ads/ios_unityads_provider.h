#pragma once

#include "ad_provider.h"

class IOSUnityAdsProvider : public AdProvider {
public:
    IOSUnityAdsProvider();
    ~IOSUnityAdsProvider();
    
    // AdProvider implementation
    void initialize(const std::string& interstitialAdId, const std::string& rewardedAdId) override;
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
    std::string gameId;  // Unity Ads requires a game ID

    std::string _interstitialAdId;
    std::string _rewardedAdId;
};