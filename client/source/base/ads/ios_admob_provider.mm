#include "pch.h"
#import <GoogleMobileAds/GoogleMobileAds.h>
#include "ios_admob_provider.h"
#include "services/ad_service.h"
#include "services/service_manager.h"

// Objective-C wrapper for Google AdMob
@interface IOSAdMobWrapper : NSObject <GADFullScreenContentDelegate>
@property(nonatomic, strong) GADRewardedAd *rewardedAd;
@property(nonatomic, strong) GADInterstitialAd *interstitialAd;
@property(nonatomic, assign) AdService* adService;
@property(nonatomic, strong) NSString* rewardedAdId;
@property(nonatomic, strong) NSString* interstitialAdId;
@property(nonatomic, assign) BOOL isInitialized;
@property(nonatomic, assign) BOOL isRewardedAdLoading;
@property(nonatomic, assign) BOOL isInterstitialAdLoading;

- (void)initializeAdMob;
- (void)loadRewardedAd;
- (void)loadInterstitialAd;
- (void)showRewardedAdFromViewController:(UIViewController *)viewController;
- (void)showInterstitialAdFromViewController:(UIViewController *)viewController;

@end

@implementation IOSAdMobWrapper

- (instancetype)init {
    self = [super init];
    if (self) {
        self.isInitialized = NO;
        self.isRewardedAdLoading = NO;
        self.isInterstitialAdLoading = NO;
    }
    return self;
}

- (void)initializeAdMob {
    [GADMobileAds.sharedInstance startWithCompletionHandler:^(GADInitializationStatus *status) {
        NSLog(@"AdMob initialization complete");
        self.isInitialized = YES;
        
        if (self.adService) {
            self.adService->onAdEvent("provider_initialized", true, "AdMob initialized successfully");
        }
        
        // Pre-load ads after initialization
        [self loadRewardedAd];
        [self loadInterstitialAd];
    }];
}

- (void)loadRewardedAd {
    if (!self.isInitialized || !self.rewardedAdId || self.isRewardedAdLoading) {
        return;
    }
    
    self.isRewardedAdLoading = YES;
    GADRequest *request = [GADRequest request];
    
    [GADRewardedAd loadWithAdUnitID:self.rewardedAdId
                            request:request
                  completionHandler:^(GADRewardedAd *ad, NSError *error) {
        self.isRewardedAdLoading = NO;
        
        if (error) {
            NSLog(@"Failed to load rewarded ad with error: %@", error.localizedDescription);
            if (self.adService) {
                self.adService->onAdEvent("rewarded_loaded", false, error.localizedDescription.UTF8String);
            }
            return;
        }
        
        self.rewardedAd = ad;
        self.rewardedAd.fullScreenContentDelegate = self;
        NSLog(@"Rewarded ad loaded successfully");
        
        if (self.adService) {
            self.adService->onAdEvent("rewarded_loaded", true, "AdMob rewarded ad loaded");
        }
    }];
}

- (void)loadInterstitialAd {
    if (!self.isInitialized || !self.interstitialAdId || self.isInterstitialAdLoading) {
        return;
    }
    
    self.isInterstitialAdLoading = YES;
    GADRequest *request = [GADRequest request];
    
    [GADInterstitialAd loadWithAdUnitID:self.interstitialAdId
                                request:request
                      completionHandler:^(GADInterstitialAd *ad, NSError *error) {
        self.isInterstitialAdLoading = NO;
        
        if (error) {
            NSLog(@"Failed to load interstitial ad with error: %@", error.localizedDescription);
            if (self.adService) {
                self.adService->onAdEvent("interstitial_loaded", false, error.localizedDescription.UTF8String);
            }
            return;
        }
        
        self.interstitialAd = ad;
        self.interstitialAd.fullScreenContentDelegate = self;
        NSLog(@"Interstitial ad loaded successfully");
        
        if (self.adService) {
            self.adService->onAdEvent("interstitial_loaded", true, "AdMob interstitial ad loaded");
        }
    }];
}

- (void)showRewardedAdFromViewController:(UIViewController *)viewController {
    if (self.rewardedAd) {
        [self.rewardedAd presentFromRootViewController:viewController 
                              userDidEarnRewardHandler:^{
            GADAdReward *reward = self.rewardedAd.adReward;
            int amount = [reward.amount intValue];
            NSString *type = reward.type;
            NSLog(@"User earned reward: %d %@", amount, type);
            
            if (self.adService) {
                self.adService->onRewardEarned(amount, type.UTF8String);
            }
        }];
    } else {
        NSLog(@"Rewarded ad is not ready yet.");
        if (self.adService) {
            self.adService->onAdEvent("rewarded_shown", false, "AdMob rewarded ad not ready");
        }
        // Try to load for next time
        [self loadRewardedAd];
    }
}

- (void)showInterstitialAdFromViewController:(UIViewController *)viewController {
    if (self.interstitialAd) {
        [self.interstitialAd presentFromRootViewController:viewController];
    } else {
        NSLog(@"Interstitial ad is not ready yet.");
        if (self.adService) {
            self.adService->onAdEvent("interstitial_shown", false, "AdMob interstitial ad not ready");
        }
        // Try to load for next time
        [self loadInterstitialAd];
    }
}

#pragma mark - GADFullScreenContentDelegate

- (void)adDidPresentFullScreenContent:(id)ad {
    NSLog(@"Ad did present full screen content.");
    
    if (self.adService) {
        if (ad == self.rewardedAd) {
            self.adService->onAdEvent("rewarded_shown", true, "AdMob rewarded ad shown");
        } else if (ad == self.interstitialAd) {
            self.adService->onAdEvent("interstitial_shown", true, "AdMob interstitial ad shown");
        }
    }
}

- (void)ad:(id)ad didFailToPresentFullScreenContentWithError:(NSError *)error {
    NSLog(@"Ad failed to present full screen content with error: %@", error.localizedDescription);
    
    if (self.adService) {
        if (ad == self.rewardedAd) {
            self.adService->onAdEvent("rewarded_shown", false, error.localizedDescription.UTF8String);
        } else if (ad == self.interstitialAd) {
            self.adService->onAdEvent("interstitial_shown", false, error.localizedDescription.UTF8String);
        }
    }
}

- (void)adDidDismissFullScreenContent:(id)ad {
    NSLog(@"Ad did dismiss full screen content.");
    
    if (self.adService) {
        if (ad == self.rewardedAd) {
            // Clear and reload for next time
            self.rewardedAd = nil;
            [self loadRewardedAd];
        } else if (ad == self.interstitialAd) {
            // Clear and reload for next time
            self.interstitialAd = nil;
            [self loadInterstitialAd];
        }
    }
}

- (void)adWillDismissFullScreenContent:(id)ad {
    NSLog(@"Ad will dismiss full screen content.");
}

- (void)adDidRecordImpression:(id)ad {
    NSLog(@"Ad did record impression.");
}

- (void)adDidRecordClick:(id)ad {
    NSLog(@"Ad did record click.");
}

@end

// C++ Implementation
IOSAdMobProvider::IOSAdMobProvider() 
    : isInitialized(false) {
    
    IOSAdMobWrapper* wrapper = [[IOSAdMobWrapper alloc] init];
    wrapper.adService = ServiceManager::getInstance()->findService<AdService>();
    platformProvider = (__bridge_retained void*)wrapper;
}

IOSAdMobProvider::~IOSAdMobProvider() {
    if (platformProvider) {
        IOSAdMobWrapper* wrapper = (__bridge_transfer IOSAdMobWrapper*)platformProvider;
        wrapper = nil;
    }
}

void IOSAdMobProvider::initialize(const std::unordered_map<std::string, std::string>& properties) {
    auto it_interstitialAdId = properties.find("interstitialAdId");
    std::string interstitialAdId = it_interstitialAdId == properties.end() ? "" : it_interstitialAdId->second;

    auto it_rewardedAdId = properties.find("rewardedAdId");
    std::string rewardedAdId = it_rewardedAdId == properties.end() ? "" : it_rewardedAdId->second;

    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    wrapper.rewardedAdId = [NSString stringWithUTF8String:rewardedAdId.c_str()];
    wrapper.interstitialAdId = [NSString stringWithUTF8String:interstitialAdId.c_str()];
    
    [wrapper initializeAdMob];
    isInitialized = true;
}

void IOSAdMobProvider::loadRewardedAd() {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    [wrapper loadRewardedAd];
}

void IOSAdMobProvider::loadInterstitialAd() {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    [wrapper loadInterstitialAd];
}

void IOSAdMobProvider::showRewardedAd() {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showRewardedAdFromViewController:rootVC];
}

void IOSAdMobProvider::showInterstitialAd() {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    UIViewController* rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    [wrapper showInterstitialAdFromViewController:rootVC];
}

bool IOSAdMobProvider::isRewardedAdLoaded() const {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    return wrapper.rewardedAd != nil;
}

bool IOSAdMobProvider::isInterstitialAdLoaded() const {
    IOSAdMobWrapper* wrapper = (__bridge IOSAdMobWrapper*)platformProvider;
    return wrapper.interstitialAd != nil;
}