#include "pch.h"
#import <YandexMobileAds/YandexMobileAds.h>
#include "ios_yandexads_provider.h"
#include "services/ad_service.h"
#include "services/service_manager.h"

// Objective-C wrapper for Yandex Ads
@interface IOSYandexAdsWrapper : NSObject <YMARewardedAdDelegate, YMAInterstitialAdDelegate>
@property(nonatomic, strong) YMARewardedAd *rewardedAd;
@property(nonatomic, strong) YMAInterstitialAd *interstitialAd;
@property(nonatomic, assign) AdService* adService;

- (void)loadRewardedAdWithAdUnitId:(NSString *)adUnitId;
- (void)loadInterstitialAdWithAdUnitId:(NSString *)adUnitId;
- (void)showRewardedAdFromViewController:(UIViewController *)viewController;
- (void)showInterstitialAdFromViewController:(UIViewController *)viewController;

@end


@implementation IOSYandexAdsWrapper

- (instancetype)init {
    self = [super init];
    return self;
}

- (void)loadRewardedAdWithAdUnitId:(NSString *)adUnitId {
    self.rewardedAd = [[YMARewardedAd alloc] initWithAdUnitID:adUnitId];
    self.rewardedAd.delegate = self;
    [self.rewardedAd load];
}

- (void)showRewardedAdFromViewController:(UIViewController *)viewController {
    if (self.rewardedAd) {
        [self.rewardedAd presentFromViewController:viewController];
    } else {
        NSLog(@"Yandex rewarded ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("rewarded", false, "Yandex rewarded ad not ready");
        }
    }
}

- (void)loadInterstitialAdWithAdUnitId:(NSString *)adUnitId {
    self.interstitialAd = [[YMAInterstitialAd alloc] initWithAdUnitID:adUnitId];
    self.interstitialAd.delegate = self;
    [self.interstitialAd load];
}

- (void)showInterstitialAdFromViewController:(UIViewController *)viewController {
    if (self.interstitialAd) {
        [self.interstitialAd presentFromViewController:viewController];
    } else {
        NSLog(@"Yandex interstitial ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("interstitial", false, "Yandex interstitial ad not ready");
        }
    }
}

#pragma mark - YMARewardedAdDelegate

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didReward:(id<YMAReward>)reward {
    NSLog(@"Yandex Ads reward earned: %d", static_cast<int>(reward.amount));
    if (self.adService) {
        self.adService->onRewardEarned(static_cast<int>(reward.amount), "yandex_reward");
    }
}

- (void)rewardedAdDidLoad:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad loaded successfully");
    if (self.adService) {
        self.adService->onAdEvent("rewarded", true, "Yandex rewarded ad loaded");
    }
}

- (void)rewardedAdDidFailToLoad:(YMARewardedAd *)rewardedAd error:(NSError *)error {
    if (error) {
        NSLog(@"Failed to load Yandex rewarded ad: %@", error.localizedDescription);
        if (self.adService) {
            self.adService->onAdEvent("rewarded", false, error.localizedDescription.UTF8String);
        }
    }
}

- (void)rewardedAdDidFailToPresent:(YMARewardedAd *)rewardedAd error:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("rewarded", false, error.localizedDescription.UTF8String);
    }
}

- (void)rewardedAdDidAppear:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad shown");
    if (self.adService) {
        self.adService->onAdEvent("rewarded", true, "Yandex rewarded ad shown");
    }
}

- (void)rewardedAdDidDisappear:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad dismissed");
    // Reload for next time
    [self.rewardedAd load];
}

#pragma mark - YMAInterstitialAdDelegate

- (void)interstitialAdDidLoad:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad loaded successfully");
    if (self.adService) {
        self.adService->onAdEvent("interstitial", true, "Yandex interstitial ad loaded");
    }
}

- (void)interstitialAdDidFailToLoad:(YMAInterstitialAd *)interstitialAd error:(NSError *)error{
    if (error) {
        NSLog(@"Failed to load Yandex interstitial ad: %@", error.localizedDescription);
        if (self.adService) {
            self.adService->onAdEvent("interstitial", false, error.localizedDescription.UTF8String);
        }
    }
}

- (void)interstitialAdDidFailToPresent:(YMAInterstitialAd *)interstitialAd error:(NSError *)error {
    NSLog(@"Yandex interstitial ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("interstitial", false, error.localizedDescription.UTF8String);
    }
}

- (void)interstitialAdDidAppear:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad shown");
    if (self.adService) {
        self.adService->onAdEvent("interstitial", true, "Yandex interstitial ad shown");
    }
}

- (void)interstitialAdDidDisappear:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad dismissed");
    // Reload for next time
    [self.interstitialAd load];
}

@end

// C++ Implementation
IOSYandexAdsProvider::IOSYandexAdsProvider() 
    : isInitialized(false) {
    
    IOSYandexAdsWrapper* wrapper = [[IOSYandexAdsWrapper alloc] init];
    wrapper.adService = ServiceManager::getInstance()->findService<AdService>();
    platformProvider = (__bridge_retained void*)wrapper;
}

IOSYandexAdsProvider::~IOSYandexAdsProvider() {
    if (platformProvider) {
        IOSYandexAdsWrapper* wrapper = (__bridge_transfer IOSYandexAdsWrapper*)platformProvider;
        wrapper = nil;
    }
}

void IOSYandexAdsProvider::initialize(const std::unordered_map<std::string, std::string>& properties) {
    isInitialized = true;

    auto it_interstitialAdId = properties.find("interstitialAdId");
    std::string interstitialAdId = it_interstitialAdId == properties.end() ? "" : it_interstitialAdId->second;

    auto it_rewardedAdId = properties.find("rewardedAdId");
    std::string rewardedAdId = it_rewardedAdId == properties.end() ? "" : it_rewardedAdId->second;

    loadRewardedAd();
    loadInterstitialAd();
}

void IOSYandexAdsProvider::loadRewardedAd() {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    [wrapper loadRewardedAdWithAdUnitId:[NSString stringWithUTF8String:_rewardedAdId.c_str()]];
}

void IOSYandexAdsProvider::loadInterstitialAd() {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    [wrapper loadInterstitialAdWithAdUnitId:[NSString stringWithUTF8String:_interstitialAdId.c_str()]];
}

void IOSYandexAdsProvider::showRewardedAd() {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showRewardedAdFromViewController:rootVC];
}

void IOSYandexAdsProvider::showInterstitialAd() {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showInterstitialAdFromViewController:rootVC];
}

bool IOSYandexAdsProvider::isRewardedAdLoaded() const {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    return wrapper.rewardedAd != nil;
}

bool IOSYandexAdsProvider::isInterstitialAdLoaded() const {
    IOSYandexAdsWrapper* wrapper = (__bridge IOSYandexAdsWrapper*)platformProvider;
    return wrapper.interstitialAd != nil;
}