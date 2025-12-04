#include "pch.h"
#include "ad_service.h"
#include "ads/ad_provider.h"
#include "ads/android_ad_provider.h"
#include "ads/ios_yandexads_provider.h"
#include "ads/ios_unityads_provider.h"
#include "ads/ios_admob_provider.h"
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

    if (strcmp(gameLocale, "ru") == 0)
        _currentProvider.reset(createProvider("YandexAds"));
    else if (strcmp(gameLocale, "zh_Hans") == 0)
        _currentProvider.reset(createProvider("UnityAds"));
    else
        _currentProvider.reset(createProvider("AdMob"));

    if (_currentProvider)
    {
        std::unordered_map<std::string, std::string> properties;

        gameplay::Properties* configNamespace = gameplay::Game::getInstance()->getConfig();
        const gameplay::Properties* adProvider = nullptr;
        while ((adProvider = configNamespace->getNextNamespace()) != 0)
        {
            if (strcmp(adProvider->getNamespace(), "adProvider") != 0)
                continue;

            if (_currentProvider->getProviderName() != adProvider->getString("type", ""))
                continue;

            const char* system = nullptr;
#if defined(__ANDROID__)
            system = "Android";
#elif defined(__APPLE__) && TARGET_OS_IPHONE
            system = "iOS";
#endif

            gameplay::Properties* sys = adProvider->getNamespace(system, true);
            if (sys)
            {
                const char* prop = nullptr;

                while ((prop = sys->getNextProperty()) != nullptr)
                    properties[prop] = sys->getString();
            }

            break;
        }

        _currentProvider->initialize(properties);
    }

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
#elif defined(__APPLE__) && TARGET_OS_IPHONE
    if (type == "YandexAds" || type == "UnityAds")
        return new IOSUnityAdsProvider();
    return new IOSAdMobProvider();
#endif

    return nullptr;
}