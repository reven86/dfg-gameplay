#pragma once

#include "ad_provider.h"

class IOSAdMobProvider : public AdProvider {
public:
    IOSAdMobProvider();
    ~IOSAdMobProvider();
    
    // AdProvider implementation
    void initialize(const std::unordered_map<std::string, std::string>& properties) override;
    void loadRewardedAd() override;
    void loadInterstitialAd() override;
    void showRewardedAd() override;
    void showInterstitialAd() override;
    bool isRewardedAdLoaded() const override;
    bool isInterstitialAdLoaded() const override;

    virtual std::string getProviderName() const override { return "AdMob"; };
    
private:
    void* platformProvider;
    bool isInitialized;
};