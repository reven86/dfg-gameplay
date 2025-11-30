#include "pch.h"
#include "ad_service.h"
#include "ads/ad_provider.h"
#include "ads/android_ad_provider.h"
#include "main.h"




AdService::AdService(const ServiceManager* manager)
    : Service(manager)
{

}

AdService::~AdService()
{
}

bool AdService::onInit()
{
    const char * gameLocale = DfgGame::getInstance()->getGameLocale();

    if (strcmp(gameLocale, "ru") == 0 || strcmp(gameLocale, "zh_Hans") == 0)
        _currentProvider.reset(createProvider("YandexAds"));
    else
        _currentProvider.reset(createProvider("AdMob"));

    if (_currentProvider)
        _currentProvider->initialize();

    return true;
}

void AdService::onRewardEarned(int amount, const std::string& type)
{
    RewardResult reward;
    reward.amount = amount;
    reward.type = type;
    reward.success = true;

    rewardEarnedSignal(reward);
}

void AdService::onAdEvent(const std::string& eventType, bool success, const std::string& message) 
{
    adEventReceivedSignal(eventType, success, message);
}

AdProvider * AdService::createProvider(const std::string& type)
{
#ifdef __ANDROID__
    return AndroidAdProvider::create(type);
#endif

    return nullptr;
}