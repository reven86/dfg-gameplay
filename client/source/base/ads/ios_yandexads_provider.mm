#include "pch.h"
#import <YandexMobileAds/YandexMobileAds-Swift.h>
#include "ios_yandexads_provider.h"
#include "services/ad_service.h"
#include "services/service_manager.h"

// Objective-C wrapper for Yandex Ads
@interface IOSYandexAdsWrapper : NSObject <YMARewardedAdLoaderDelegate, YMAInterstitialAdLoaderDelegate, YMARewardedAdDelegate, YMAInterstitialAdDelegate>
@property(nonatomic, strong) YMARewardedAdLoader *rewardedAdLoader;
@property(nonatomic, strong) YMAInterstitialAdLoader *interstitialAdLoader;
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
    if (self) {
        // Yandex Mobile Ads SDK auto-initializes
        self.rewardedAdLoader = [[YMARewardedAdLoader alloc] init];
        self.rewardedAdLoader.delegate = self;
        
        self.interstitialAdLoader = [[YMAInterstitialAdLoader alloc] init];
        self.interstitialAdLoader.delegate = self;
    }
    return self;
}

- (void)loadRewardedAdWithAdUnitId:(NSString *)adUnitId {
    YMAAdRequestConfiguration *configuration = [[YMAAdRequestConfiguration alloc] initWithAdUnitID:adUnitId];
    [self.rewardedAdLoader loadAdWithRequestConfiguration:configuration];
}

- (void)showRewardedAdFromViewController:(UIViewController *)viewController {
    if (self.rewardedAd) {
        [self.rewardedAd showFromViewController:viewController];
    } else {
        NSLog(@"Yandex rewarded ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("rewarded_shown", false, "Yandex rewarded ad not ready");
        }
    }
}

- (void)loadInterstitialAdWithAdUnitId:(NSString *)adUnitId {
    YMAAdRequestConfiguration *configuration = [[YMAAdRequestConfiguration alloc] initWithAdUnitID:adUnitId];
    [self.interstitialAdLoader loadAdWithRequestConfiguration:configuration];
}

- (void)showInterstitialAdFromViewController:(UIViewController *)viewController {
    if (self.interstitialAd) {
        [self.interstitialAd showFromViewController:viewController];
    } else {
        NSLog(@"Yandex interstitial ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("interstitial_shown", false, "Yandex interstitial ad not ready");
        }
    }
}

#pragma mark - YMARewardedAdLoaderDelegate

- (void)rewardedAdLoader:(YMARewardedAdLoader *)adLoader didLoad:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad loaded successfully");
    self.rewardedAd = rewardedAd;
    self.rewardedAd.delegate = self;
    
    if (self.adService) {
        self.adService->onAdEvent("rewarded_loaded", true, "Yandex rewarded ad loaded");
    }
}

- (void)rewardedAdLoader:(YMARewardedAdLoader *)adLoader didFailToLoadWithError:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to load: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("rewarded_loaded", false, error.localizedDescription.UTF8String);
    }
}

#pragma mark - YMAInterstitialAdLoaderDelegate

- (void)interstitialAdLoader:(YMAInterstitialAdLoader *)adLoader didLoad:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad loaded successfully");
    self.interstitialAd = interstitialAd;
    self.interstitialAd.delegate = self;
    
    if (self.adService) {
        self.adService->onAdEvent("interstitial_loaded", true, "Yandex interstitial ad loaded");
    }
}

- (void)interstitialAdLoader:(YMAInterstitialAdLoader *)adLoader didFailToLoadWithError:(NSError *)error {
    NSLog(@"Yandex interstitial ad failed to load: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("interstitial_loaded", false, error.localizedDescription.UTF8String);
    }
}

#pragma mark - YMARewardedAdDelegate

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didReward:(id<YMAReward>)reward {
    NSLog(@"Yandex Ads reward earned: %ld", static_cast<long>(reward.amount));
    if (self.adService) {
        self.adService->onRewardEarned(static_cast<int>(reward.amount), "yandex_reward");
    }
}

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didFailToShowWithError:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("rewarded_shown", false, error.localizedDescription.UTF8String);
    }
}

- (void)rewardedAdDidShow:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad shown");
    if (self.adService) {
        self.adService->onAdEvent("rewarded_shown", true, "Yandex rewarded ad shown");
    }
}

- (void)rewardedAdDidDismiss:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad dismissed");
    // Reload for next time
    [self loadRewardedAdWithAdUnitId:rewardedAd.adInfo.adUnitId];
}

- (void)rewardedAdDidClick:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad clicked");
}

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didTrackImpressionWithData:(nullable id<YMAImpressionData>)impressionData {
    NSLog(@"Yandex rewarded ad impression tracked");
}

#pragma mark - YMAInterstitialAdDelegate

- (void)interstitialAd:(YMAInterstitialAd *)interstitialAd didFailToShowWithError:(NSError *)error {
    NSLog(@"Yandex interstitial ad failed to show: %@", error.localizedDescription);
    if (self.adService) {
        self.adService->onAdEvent("interstitial_shown", false, error.localizedDescription.UTF8String);
    }
}

- (void)interstitialAdDidShow:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad shown");
    if (self.adService) {
        self.adService->onAdEvent("interstitial_shown", true, "Yandex interstitial ad shown");
    }
}

- (void)interstitialAdDidDismiss:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad dismissed");
    // Reload for next time
    [self loadInterstitialAdWithAdUnitId:interstitialAd.adInfo.adUnitId];
}

- (void)interstitialAdDidClick:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad clicked");
}

- (void)interstitialAd:(YMAInterstitialAd *)interstitialAd didTrackImpressionWithData:(nullable id<YMAImpressionData>)impressionData {
    NSLog(@"Yandex interstitial ad impression tracked");
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