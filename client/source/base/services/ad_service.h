#pragma once

#include "service.h"




/** @brief Service for serving ads.
 */

class AdService : public Service
{
    friend class ServiceManager;

public:
    struct RewardResult
    {
        int amount = 0;
        std::string type;
        bool success = false;
    };

    sigc::signal<void, const RewardResult&> rewardEarnedSignal;
    sigc::signal<void, const std::string&, bool, const std::string&> adEventReceivedSignal;

    static const char* getTypeName() { return "AdService"; }

    class AdProvider* getProvider() const { return _currentProvider.get(); }


    // Callback handlers
    void onRewardEarned(int amount, const std::string& type);
    void onAdEvent(const std::string& eventType, bool success, const std::string& message);

protected:
    AdService(const ServiceManager* manager);
    virtual ~AdService();

    virtual bool onInit();

private:
    /**
     * Create AdProvider.
     *
     * @param type Provider type. One of AdMob, UnityAds or YandexAds
     */
    static class AdProvider * createProvider(const std::string& type);


    std::unique_ptr<class AdProvider> _currentProvider;
};

