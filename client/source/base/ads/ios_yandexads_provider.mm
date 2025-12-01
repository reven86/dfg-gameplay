#import <YandexMobileAds/YandexMobileAds.h>
#include "ios_yandexads_provider.h"

// Objective-C wrapper for Yandex Ads
@interface IOSYandexAdsWrapper : NSObject <YMARewardedAdLoaderDelegate, YMAInterstitialAdLoaderDelegate, YMARewardedAdDelegate, YMAInterstitialAdDelegate>
@property(nonatomic, strong) YMARewardedAdLoader *rewardedAdLoader;
@property(nonatomic, strong) YMAInterstitialAdLoader *interstitialAdLoader;
@property(nonatomic, strong) YMARewardedAd *rewardedAd;
@property(nonatomic, strong) YMAInterstitialAd *interstitialAd;
@property(nonatomic, assign) IOSYandexAdsProvider* cppProvider;

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
        [self.rewardedAd presentFromViewController:viewController];
    } else {
        NSLog(@"Yandex rewarded ad is not ready");
        if (self.cppProvider) {
            self.cppProvider->adEventReceivedSignal("rewarded", false, "Yandex rewarded ad not ready");
        }
    }
}

- (void)loadInterstitialAdWithAdUnitId:(NSString *)adUnitId {
    YMAAdRequestConfiguration *configuration = [[YMAAdRequestConfiguration alloc] initWithAdUnitID:adUnitId];
    [self.interstitialAdLoader loadAdWithRequestConfiguration:configuration];
}

- (void)showInterstitialAdFromViewController:(UIViewController *)viewController {
    if (self.interstitialAd) {
        [self.interstitialAd presentFromViewController:viewController];
    } else {
        NSLog(@"Yandex interstitial ad is not ready");
        if (self.cppProvider) {
            self.cppProvider->adEventReceivedSignal("interstitial", false, "Yandex interstitial ad not ready");
        }
    }
}

#pragma mark - YMARewardedAdLoaderDelegate

- (void)rewardedAdLoader:(YMARewardedAdLoader *)adLoader didLoad:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad loaded successfully");
    self.rewardedAd = rewardedAd;
    self.rewardedAd.delegate = self;
    
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("rewarded", true, "Yandex rewarded ad loaded");
    }
}

- (void)rewardedAdLoader:(YMARewardedAdLoader *)adLoader didFailToLoadWithError:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to load: %@", error.localizedDescription);
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("rewarded", false, error.localizedDescription.UTF8String);
    }
}

#pragma mark - YMAInterstitialAdLoaderDelegate

- (void)interstitialAdLoader:(YMAInterstitialAdLoader *)adLoader didLoad:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad loaded successfully");
    self.interstitialAd = interstitialAd;
    self.interstitialAd.delegate = self;
    
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("interstitial", true, "Yandex interstitial ad loaded");
    }
}

- (void)interstitialAdLoader:(YMAInterstitialAdLoader *)adLoader didFailToLoadWithError:(NSError *)error {
    NSLog(@"Yandex interstitial ad failed to load: %@", error.localizedDescription);
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("interstitial", false, error.localizedDescription.UTF8String);
    }
}

#pragma mark - YMARewardedAdDelegate

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didReward:(id<YMAReward>)reward {
    NSLog(@"Yandex Ads reward earned: %@", reward.amount);
    if (self.cppProvider) {
        self.cppProvider->rewardEarnedSignal([reward.amount intValue], "yandex_reward");
    }
}

- (void)rewardedAd:(YMARewardedAd *)rewardedAd didFailToShowWithError:(NSError *)error {
    NSLog(@"Yandex rewarded ad failed to show: %@", error.localizedDescription);
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("rewarded", false, error.localizedDescription.UTF8String);
    }
}

- (void)rewardedAdDidShow:(YMARewardedAd *)rewardedAd {
    NSLog(@"Yandex rewarded ad shown");
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("rewarded", true, "Yandex rewarded ad shown");
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
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("interstitial", false, error.localizedDescription.UTF8String);
    }
}

- (void)interstitialAdDidShow:(YMAInterstitialAd *)interstitialAd {
    NSLog(@"Yandex interstitial ad shown");
    if (self.cppProvider) {
        self.cppProvider->adEventReceivedSignal("interstitial", true, "Yandex interstitial ad shown");
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
    : rewardedAd(nullptr), interstitialAd(nullptr),
      rewardedAdLoader(nullptr),
      interstitialAdLoader(nullptr), 
      isInitialized(false) {
    
    IOSYandexAdsWrapper* wrapper = [[IOSYandexAdsWrapper alloc] init];
    wrapper.cppProvider = this;
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