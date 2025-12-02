/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>

@class YMAAdRequest;
@protocol YMAImpressionData;

NS_ASSUME_NONNULL_BEGIN

@protocol YMAInterstitialAdDelegate;
/**
 This class is responsible for loading a full-screen ad.
 */
@interface YMAInterstitialAd : NSObject

/**
 The AdUnit ID is a unique identifier in the R-M-XXXXXX-Y format, which is assigned in the Partner interface.
 */
@property (nonatomic, copy, readonly) NSString *adUnitID;

/**
 Notifies that the ad is loaded and ready to be displayed.
 @discussion After the property takes the `YES` value,
 the [YMAInterstitialAdDelegate interstitialAdDidLoad:] delegate method is called.
 */
@property (nonatomic, assign, readonly) BOOL loaded;

/**
 Tracks ad lifecycle events.
 */
@property (nonatomic, weak, nullable) id<YMAInterstitialAdDelegate> delegate;

/**
 Defines whether to open links in the app or in the browser installed on the device. The default value is `NO`.
 */
@property (nonatomic, assign) BOOL shouldOpenLinksInApp;

/**
 Notifies whether the ad was displayed.
 */
@property (nonatomic, assign, readonly) BOOL hasBeenPresented;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

/**
 Initializes an object of the YMAInterstitialAd class with a full-screen ad.
 @param adUnitID The AdUnit ID is a unique identifier in the R-M-XXXXXX-Y format,
 which is assigned in the Partner interface.
 @return An object of the YMAInterstitialAd class with a full-screen ad.
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID;

/**
 Preloads an ad.
 @discussion After this, use the [YMAInterstitialAd presentFromViewController:] method
 to start displaying the ad.
 */
- (void)load;

/**
 Preloads the ad by setting the data for targeting.
 @param request Data for targeting.
 */
- (void)loadWithRequest:(nullable YMAAdRequest *)request;

/**
 Use this method to display a full-screen ad after preloading.
 @param viewController An object of the `UIViewController` class, which is used to display a full-screen ad.
 */
- (void)presentFromViewController:(UIViewController *)viewController;

/**
 Use this method to display a full-screen ad after preloading
 if you need the app to perform some action immediately after showing the ad.
 @param viewController An object of the `UIViewController` class.
 @param dismissalBlock A block of code that executes after the ad is shown.
 */
- (void)presentFromViewController:(UIViewController *)viewController
                   dismissalBlock:(nullable void(^)(void))dismissalBlock;

@end
/**
 The protocol defines the methods of a delegate that monitors the ads.
 @discussion Methods are called by an object of the YMAInterstitialAd class when its state changes.
 */
@protocol YMAInterstitialAdDelegate <NSObject>

@optional

/**
 Notifies that the ad loaded successfully.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdDidLoad:(YMAInterstitialAd *)interstitialAd;

/**
 Notifies that the ad failed to load.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 @param error Information about the error (for details, see YMAAdErrorCode).
 */
- (void)interstitialAdDidFailToLoad:(YMAInterstitialAd *)interstitialAd error:(NSError *)error;

/**
 Notifies that the app will run in the background now because the user clicked the ad
 and is switching to a different application (Phone, App Store, and so on).
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdWillLeaveApplication:(YMAInterstitialAd *)interstitialAd;

/**
 Notifies that the ad can't be displayed.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 @param error Information about the error (for details, see YMAAdErrorCode).
 */
- (void)interstitialAdDidFailToPresent:(YMAInterstitialAd *)interstitialAd error:(NSError *)error;

/**
 Called before the full-screen ad appears.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdWillAppear:(YMAInterstitialAd *)interstitialAd;

/**
 Called after the full-screen ad appears.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdDidAppear:(YMAInterstitialAd *)interstitialAd;

/**
 Called before hiding the full-screen ad.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdWillDisappear:(YMAInterstitialAd *)interstitialAd;

/**
 Called after hiding the full-screen ad.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdDidDisappear:(YMAInterstitialAd *)interstitialAd;

/**
 Notifies that the user has clicked on the ad.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 */
- (void)interstitialAdDidClick:(YMAInterstitialAd *)interstitialAd;

/**
 Notifies that the embedded browser will be displayed.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 @param webBrowser The in-app browser.
 */
- (void)interstitialAd:(YMAInterstitialAd *)interstitialAd willPresentScreen:(nullable UIViewController *)webBrowser;

/**
 Notifies delegate when an impression was tracked.
 @param interstitialAd A reference to an object of the YMAInterstitialAd class that invoked the method.
 @param impressionData Ad impression-level revenue data.
 */
- (void)interstitialAd:(YMAInterstitialAd *)interstitialAd
        didTrackImpressionWithData:(nullable id<YMAImpressionData>)impressionData;

@end

NS_ASSUME_NONNULL_END
