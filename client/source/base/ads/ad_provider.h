#pragma once


#include <functional>
#include <string>

class AdProvider {
public:
    virtual ~AdProvider() = default;

    // Initialization
    virtual void initialize(const std::unordered_map<std::string, std::string>& properties) = 0;

    // Ad loading
    virtual void loadRewardedAd() = 0;
    virtual void loadInterstitialAd() = 0;

    // Ad showing
    virtual void showRewardedAd() = 0;
    virtual void showInterstitialAd() = 0;

    // Status checking
    virtual bool isRewardedAdLoaded() const = 0;
    virtual bool isInterstitialAdLoaded() const = 0;

    // Provider identification
    virtual std::string getProviderName() const = 0;
};
