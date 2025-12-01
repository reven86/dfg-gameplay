#pragma once

#include "ad_provider.h"

class IOSYandexAdsProvider : public AdProvider {
public:
    IOSYandexAdsProvider();
    ~IOSYandexAdsProvider();
    
    // AdProvider implementation
    void initialize(const std::string& interstitialAdId, const std::string& rewardedAdId) override;
    void loadRewardedAd() override;
    void loadInterstitialAd() override;
    void showRewardedAd() override;
    void showInterstitialAd() override;
    bool isRewardedAdLoaded() const override;
    bool isInterstitialAdLoaded() const override;

    virtual std::string getProviderName() const override { return "YandexAds"; };
    
private:
    void* platformProvider;
    void* rewardedAdLoader;      // YMARewardedAdLoader*
    void* interstitialAdLoader;  // YMAInterstitialAdLoader*
    void* rewardedAd;      // YMARewardedAd*
    void* interstitialAd;  // YMAInterstitialAd*
    bool isInitialized;

    std::string _interstitialAdId;
    std::string _rewardedAdId;
};
