#include "pch.h"
#import <YandexMobileAds_ios12/YandexMobileAds.h>
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
    YMAAdRequestConfiguration *configuration = [[YMAAdRequestConfiguration alloc] initWithAdUnitID:adUnitId];
    
    self.rewardedAd = [[YMARewardedAd alloc] initWithAdUnitID:adUnitId];
    self.rewardedAd.delegate = self;
    
    [self.rewardedAd loadWithRequestConfiguration:configuration completionHandler:^(NSError * _Nullable error) {
        if (error) {
            NSLog(@"Failed to load Yandex rewarded ad: %@", error.localizedDescription);
            if (self.adService) {
                self.adService->onAdEvent("rewarded", false, error.localizedDescription.UTF8String);
            }
        } else {
            NSLog(@"Yandex rewarded ad loaded successfully");
            if (self.adService) {
                self.adService->onAdEvent("rewarded", true, "Yandex rewarded ad loaded");
            }
        }
    }];
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
    YMAAdRequestConfiguration *configuration = [[YMAAdRequestConfiguration alloc] initWithAdUnitID:adUnitId];
    
    self.interstitialAd = [[YMAInterstitialAd alloc] initWithAdUnitID:adUnitId];
    self.interstitialAd.delegate = self;
    
    [self.interstitialAd loadWithRequestConfiguration:configuration completionHandler:^(NSError * _Nullable error) {
        if (error) {
            NSLog(@"Failed to load Yandex interstitial ad: %@", error.localizedDescription);
            if (self.adService) {
                self.adService->onAdEvent("interstitial", false, error.localizedDescription.UTF8String);
            }
        } else {
            NSLog(@"Yandex interstitial ad loaded successfully");
            if (self.adService) {
                self.adService->onAdEvent("interstitial", true, "Yandex interstitial ad loaded");
            }
        }
    }];
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
    NSLog(@"Yandex Ads reward earned: %@", reward.amount);
    if (self.adService) {
        self.adService->onRewardEarned([reward.amount intValue], "yandex_reward");
    }
}

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didFailToShowWithError:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("rewarded", false, error.localizedDescription.UTF8String);
    }
}

- (void)rewardedAdDidShow:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad shown");
    if (self.adService) {
        self.adService->onAdEvent("rewarded", true, "Yandex rewarded ad shown");
    }
}

- (void)rewardedAdDidDismiss:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad dismissed");
    // Reload for next time
    [self loadRewardedAdWithAdUnitId:rewardedAd.adInfo.adUnitId];
}

#pragma mark - YMAInterstitialAdDelegate

- (void)interstitialAd:(YMAInterstitialAd *)interstitialAd didFailToShowWithError:(NSError *)error {
    NSLog(@"Yandex interstitial ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("interstitial", false, error.localizedDescription.UTF8String);
    }
}

- (void)interstitialAdDidShow:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad shown");
    if (self.adService) {
        self.adService->onAdEvent("interstitial", true, "Yandex interstitial ad shown");
    }
}

- (void)interstitialAdDidDismiss:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad dismissed");
    // Reload for next time
    [self loadInterstitialAdWithAdUnitId:interstitialAd.adInfo.adUnitId];
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

void IOSYandexAdsProvider::initialize(const std::string& interstitialAdId, const std::string& rewardedAdId) {
    isInitialized = true;
    _interstitialAdId = interstitialAdId;
    _rewardedAdId = rewardedAdId;
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