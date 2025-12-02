#include "pch.h"
#import <UnityAds/UnityAds.h>
#import <UnityAds/UnityAds-Swift.h>
#include "ios_unityads_provider.h"
#include "services/ad_service.h"
#include "services/service_manager.h"

// Forward declarations for Unity Ads types
@protocol UnityAdsInitializationDelegate;
@protocol UnityAdsLoadDelegate;
@protocol UnityAdsShowDelegate;

// Objective-C wrapper for Unity Ads
@interface IOSUnityAdsWrapper : NSObject <UnityAdsInitializationDelegate, UnityAdsLoadDelegate, UnityAdsShowDelegate>
@property(nonatomic, assign) BOOL isInitialized;
@property(nonatomic, strong) NSString* gameId;
@property(nonatomic, assign) BOOL testMode;
@property(nonatomic, strong) NSString* rewardedAdId;
@property(nonatomic, strong) NSString* interstitialAdId;
@property(nonatomic, assign) AdService* adService;

- (instancetype)init;
- (void)initializeUnityAds;
- (void)loadRewardedAd;
- (void)loadInterstitialAd;
- (void)showRewardedAdFromViewController:(UIViewController *)viewController;
- (void)showInterstitialAdFromViewController:(UIViewController *)viewController;
- (BOOL)isRewardedAdReady;
- (BOOL)isInterstitialAdReady;

@end

@implementation IOSUnityAdsWrapper

- (instancetype)init {
    self = [super init];
    if (self) {
        self.testMode = NO;
        self.isInitialized = NO;
    }
    return self;
}

- (void)initializeUnityAds {
    [UnityAds initialize:self.gameId 
                testMode:self.testMode 
      initializationDelegate:self];
}

- (void)loadRewardedAd {
    if (self.isInitialized && self.rewardedAdId) {
        [UnityAds load:self.rewardedAdId loadDelegate:self];
    }
}

- (void)loadInterstitialAd {
    if (self.isInitialized && self.interstitialAdId) {
        [UnityAds load:self.interstitialAdId loadDelegate:self];
    }
}

- (void)showRewardedAdFromViewController:(UIViewController *)viewController {
    if ([self isRewardedAdReady]) {
        [UnityAds show:viewController 
           placementId:self.rewardedAdId 
           showDelegate:self];
    } else {
        NSLog(@"Unity rewarded ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("rewarded", false, "Unity rewarded ad not ready");
        }
    }
}

- (void)showInterstitialAdFromViewController:(UIViewController *)viewController {
    if ([self isInterstitialAdReady]) {
        [UnityAds show:viewController 
           placementId:self.interstitialAdId 
           showDelegate:self];
    } else {
        NSLog(@"Unity interstitial ad is not ready");
        if (self.adService) {
            self.adService->onAdEvent("interstitial", false, "Unity interstitial ad not ready");
        }
    }
}

- (BOOL)isRewardedAdReady {
    return self.isInitialized && self.rewardedAdId;
}

- (BOOL)isInterstitialAdReady {
    return self.isInitialized && self.interstitialAdId;
}

#pragma mark - UnityAdsInitializationDelegate

- (void)initializationComplete {
    NSLog(@"Unity Ads initialization complete");
    self.isInitialized = YES;
    
    if (self.adService) {
        self.adService->onAdEvent("provider_initialized", true, "Unity Ads initialized successfully");
    }
    
    // Pre-load ads after initialization
    [self loadRewardedAd];
    [self loadInterstitialAd];
}

- (void)initializationFailed:(UnityAdsInitializationError)error withMessage:(NSString *)message {
    NSLog(@"Unity Ads initialization failed: %@", message);
    if (self.adService) {
        self.adService->onAdEvent("provider_initialized", false, message.UTF8String);
    }
}

#pragma mark - UnityAdsLoadDelegate

- (void)unityAdsAdLoaded:(NSString *)placementId {
    NSLog(@"Unity Ads ad loaded: %@", placementId);
    
    if (self.adService) {
        std::string adType = [placementId isEqualToString:self.rewardedAdId] ? "rewarded" : "interstitial";
        self.adService->onAdEvent(adType, true, "Unity Ads loaded");
    }
}

- (void)unityAdsAdFailedToLoad:(NSString *)placementId 
                     withError:(UnityAdsLoadError)error 
                   withMessage:(NSString *)message {
    NSLog(@"Unity Ads failed to load ad %@: %@", placementId, message);
    
    if (self.adService) {
        std::string adType = [placementId isEqualToString:self.rewardedAdId] ? "rewarded" : "interstitial";
        self.adService->onAdEvent(adType, false, message.UTF8String);
    }
}

#pragma mark - UnityAdsShowDelegate

- (void)unityAdsShowStart:(NSString *)placementId {
    NSLog(@"Unity Ads show started: %@", placementId);
    
    if (self.adService) {
        std::string adType = [placementId isEqualToString:self.rewardedAdId] ? "rewarded" : "interstitial";
        self.adService->onAdEvent(adType, true, "Unity Ads shown");
    }
}

- (void)unityAdsShowFailed:(NSString *)placementId 
                 withError:(UnityAdsShowError)error 
               withMessage:(NSString *)message {
    NSLog(@"Unity Ads show failed for %@: %@", placementId, message);
    
    if (self.adService) {
        std::string adType = [placementId isEqualToString:self.rewardedAdId] ? "rewarded" : "interstitial";
        self.adService->onAdEvent(adType, false, message.UTF8String);
    }
}

- (void)unityAdsShowClick:(NSString *)placementId {
    NSLog(@"Unity Ads ad clicked: %@", placementId);
    // Optional: Track click events
}

- (void)unityAdsShowComplete:(NSString *)placementId 
             withFinishState:(UnityAdsShowCompletionState)state {
    NSLog(@"Unity Ads show completed: %@ with state: %ld", placementId, (long)state);
    
    if (self.adService) {
        // Handle reward for rewarded ads
        if ([placementId isEqualToString:self.rewardedAdId] && 
            state == kUnityShowCompletionStateCompleted) {
            // Unity Ads doesn't provide reward amount/type, use defaults
            self.adService->onRewardEarned(1, "unity_reward");
        }
        
        // Reload the ad that was shown
        if ([placementId isEqualToString:self.rewardedAdId]) {
            [self loadRewardedAd];
        } else if ([placementId isEqualToString:self.interstitialAdId]) {
            [self loadInterstitialAd];
        }
    }
}

@end

// C++ Implementation
IOSUnityAdsProvider::IOSUnityAdsProvider() 
    : isInitialized(false) {
    
    IOSUnityAdsWrapper* wrapper = [[IOSUnityAdsWrapper alloc] init];
    wrapper.adService = ServiceManager::getInstance()->findService<AdService>();
    platformProvider = (__bridge_retained void*)wrapper;
}

IOSUnityAdsProvider::~IOSUnityAdsProvider() {
    if (platformProvider) {
        IOSUnityAdsWrapper* wrapper = (__bridge_transfer IOSUnityAdsWrapper*)platformProvider;
        wrapper = nil;
    }
}

void IOSUnityAdsProvider::initialize(const std::unordered_map<std::string, std::string>& properties) {

    auto it_interstitialAdId = properties.find("interstitialAdId");
    std::string interstitialAdId = it_interstitialAdId == properties.end() ? "" : it_interstitialAdId->second;

    auto it_rewardedAdId = properties.find("rewardedAdId");
    std::string rewardedAdId = it_rewardedAdId == properties.end() ? "" : it_rewardedAdId->second;
        
    auto it_gameId = properties.find("gameId");
    std::string gameId = it_gameId == properties.end() ? "" : it_gameId->second;
 
    GP_LOG("IOSUnityAdsProvider::initialize %s %s %s %X", gameId.c_str(), interstitialAdId.c_str(), rewardedAdId.c_str(), platformProvider);
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    wrapper.gameId = [NSString stringWithUTF8String:gameId.c_str()];
    wrapper.testMode = false;
    wrapper.rewardedAdId = [NSString stringWithUTF8String:rewardedAdId.c_str()];
    wrapper.interstitialAdId = [NSString stringWithUTF8String:interstitialAdId.c_str()];
    
    [wrapper initializeUnityAds];
    isInitialized = true;
}

void IOSUnityAdsProvider::loadRewardedAd() {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    [wrapper loadRewardedAd];
}

void IOSUnityAdsProvider::loadInterstitialAd() {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    [wrapper loadInterstitialAd];
}

void IOSUnityAdsProvider::showRewardedAd() {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showRewardedAdFromViewController:rootVC];
}

void IOSUnityAdsProvider::showInterstitialAd() {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showInterstitialAdFromViewController:rootVC];
}

bool IOSUnityAdsProvider::isRewardedAdLoaded() const {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    return [wrapper isRewardedAdReady];
}

bool IOSUnityAdsProvider::isInterstitialAdLoaded() const {
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    return [wrapper isInterstitialAdReady];
}