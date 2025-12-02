#include "pch.h"
#import <UnityAds/UnityAds.h>
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

- (instancetype)initWithGameId:(NSString*)gameId testMode:(BOOL)testMode;
- (void)initializeUnityAds;
- (void)loadRewardedAd;
- (void)loadInterstitialAd;
- (void)showRewardedAdFromViewController:(UIViewController *)viewController;
- (void)showInterstitialAdFromViewController:(UIViewController *)viewController;
- (BOOL)isRewardedAdReady;
- (BOOL)isInterstitialAdReady;

@end

@implementation IOSUnityAdsWrapper

- (instancetype)initWithGameId:(NSString*)gameId testMode:(BOOL)testMode {
    self = [super init];
    if (self) {
        self.gameId = gameId;
        self.testMode = testMode;
        self.isInitialized = NO;
    }
    return self;
}

- (void)initializeUnityAds {
    [UnityServices initialize:self.gameId 
                testMode:self.testMode 
      initializationDelegate:self];
}

- (void)loadRewardedAd {
    if (self.isInitialized && self.rewardedAdId) {
        [UnityServices load:self.rewardedAdId loadDelegate:self];
    }
}

- (void)loadInterstitialAd {
    if (self.isInitialized && self.interstitialAdId) {
        [UnityServices load:self.interstitialAdId loadDelegate:self];
    }
}

- (void)showRewardedAdFromViewController:(UIViewController *)viewController {
    if ([self isRewardedAdReady]) {
        [UnityServices show:viewController 
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
        [UnityServices show:viewController 
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
    
    // Note: Unity Ads requires a game ID, which should be passed in initialize()
    // We'll create the wrapper with default values for now
    IOSUnityAdsWrapper* wrapper = [[IOSUnityAdsWrapper alloc] initWithGameId:@"" testMode:NO];
    wrapper.adService = ServiceManager::getInstance()->findService<AdService>();
    platformProvider = (__bridge_retained void*)wrapper;
}

IOSUnityAdsProvider::~IOSUnityAdsProvider() {
    if (platformProvider) {
        IOSUnityAdsWrapper* wrapper = (__bridge_transfer IOSUnityAdsWrapper*)platformProvider;
        wrapper = nil;
    }
}

void IOSUnityAdsProvider::initialize(const std::string& interstitialAdId, const std::string& rewardedAdId) {
    _interstitialAdId = interstitialAdId;
    _rewardedAdId = rewardedAdId;
    
    // Unity Ads requires a game ID - you need to configure this somehow
    // Options:
    // 1. Pass game ID as part of ad ID string (e.g., "gameId:adUnitId")
    // 2. Store game ID in configuration
    // 3. Use a default or parse from somewhere
    
    // For now, we'll assume the game ID is passed as the first part before ':'
    std::string gameIdStr = "0"; // Default/test game ID
    bool testMode = false;
    
    // Parse game ID and test mode if provided in a special format
    // Example format: "1234567:true" or just "1234567"
    if (!_rewardedAdId.empty()) {
        size_t colonPos = _rewardedAdId.find(':');
        if (colonPos != std::string::npos) {
            gameIdStr = _rewardedAdId.substr(0, colonPos);
            std::string testModeStr = _rewardedAdId.substr(colonPos + 1);
            testMode = (testModeStr == "true" || testModeStr == "1");
            
            // Remove game ID prefix from actual ad IDs
            _rewardedAdId = _rewardedAdId.substr(colonPos + 1);
            size_t secondColon = _rewardedAdId.find(':');
            if (secondColon != std::string::npos) {
                _rewardedAdId = _rewardedAdId.substr(secondColon + 1);
            }
        }
    }
    
    IOSUnityAdsWrapper* wrapper = (__bridge IOSUnityAdsWrapper*)platformProvider;
    wrapper.gameId = [NSString stringWithUTF8String:gameIdStr.c_str()];
    wrapper.testMode = testMode;
    wrapper.rewardedAdId = [NSString stringWithUTF8String:_rewardedAdId.c_str()];
    wrapper.interstitialAdId = [NSString stringWithUTF8String:_interstitialAdId.c_str()];
    
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