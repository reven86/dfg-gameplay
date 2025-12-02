/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>

@class YMAAdRequest;
@class YMAAdSize;
@class YMAVideoController;

@protocol YMAAdViewDelegate;
@protocol YMAImpressionData;

NS_ASSUME_NONNULL_BEGIN
/**
 This class is responsible for setting up and displaying the banner.
 */
@interface YMAAdView : UIView

/**
 It monitors the ad and receives notifications about user interaction with the ad.
 @discussion It provides data necessary for displaying the ad
 (for example, a `UIViewController` object that `AdView` uses to show a modal controller
 in response to the user interacting with the banner).
 */
@property (nonatomic, weak, nullable) id<YMAAdViewDelegate> delegate;

/**
 The AdUnit ID is a unique identifier in the R-M-XXXXXX-Y format, which is assigned in the Partner interface.
 */
@property (nonatomic, copy, readonly) NSString *adUnitID;

/**
 VideoController provides playback control for ad video.
 */
@property (nonatomic, strong, readonly) YMAVideoController *videoController;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

- (instancetype)initWithFrame:(CGRect)frame NS_UNAVAILABLE;

- (instancetype)initWithCoder:(NSCoder *)aDecoder NS_UNAVAILABLE;
/**
 Initializes an object of the YMAAdView class to display the banner with the specified size.
 @param adUnitID The AdUnit ID is a unique identifier in the R-M-XXXXXX-Y format,
 which is assigned in the Partner interface.
 @param adSize The size of the banner. The size of the banner is set in the YMAAdSize class.
 @return Initializes an object of the YMAAdView class to display the banner with the specified size.
 */
- (instancetype)initWithAdUnitID:(NSString *)adUnitID adSize:(YMAAdSize *)adSize;

/**
 Displays the banner centered at the top of the passed `View`.
 @param view Object of the `UIView` class to add the banner to.
 */
- (void)displayAtTopInView:(UIView *)view;

/**
 Displays the banner centered at the bottom of the passed `View`.
 @param view An object of the `UIView` class to add the banner to.
 */
- (void)displayAtBottomInView:(UIView *)view;

/**
 Loads a banner.
 */
- (void)loadAd;

/**
 Loads a banner with data for targeting.
 @param request Data for targeting.
 */
- (void)loadAdWithRequest:(nullable YMAAdRequest *)request;

/**
 Returns the size of the banner content.
 @return The size of the banner content.
 */
- (CGSize)adContentSize;

@end

/**
 The protocol defines the methods of a delegate that monitors the ads.
 @discussion Methods are called by an object of the YMAAdView class when its state changes.
 */
@protocol YMAAdViewDelegate <NSObject>

@optional

/**
 Returns a `UIViewController` object that `AdView` uses to show a modal controller
 in response to the user's interaction with the banner.
 @discussion Since a single `UIViewController` can't show multiple modal controllers simultaneously,
 the returned `UIViewController` must be displayed in front of all the other ones.
 @return The `UIViewController` object that `AdView` uses for showing a modal controller
 in response to the user's interaction with the banner.
 */
- (nullable UIViewController *)viewControllerForPresentingModalView;

/**
 Notifies that the banner is loaded.
 @discussion At this time, you can add `AdView` if you haven't done so yet.
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 */
- (void)adViewDidLoad:(YMAAdView *)adView;

/**
 Notifies that the banner failed to load.
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 @param error Information about the error (for details, see YMAAdErrorCode).
 */
- (void)adViewDidFailLoading:(YMAAdView *)adView error:(NSError *)error;

/**
 Notifies that the user has clicked on the banner.
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 */
- (void)adViewDidClick:(YMAAdView *)adView;

/**
 Notifies that the app will become inactive now because the user clicked on the banner ad
 and is about to switch to a different application (Phone, App Store, and so on).
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 */
- (void)adViewWillLeaveApplication:(YMAAdView *)adView;

/**
 Notifies that the user has clicked on the banner and the in-app browser will open now.
 @param adView A reference to the object of the YMAAdView class that invoked the method. 
 @param viewController Modal `UIViewController`.
 */
- (void)adView:(YMAAdView *)adView willPresentScreen:(nullable UIViewController *)viewController;

/**
 Notifies that the user has closed the embedded browser.
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 @param viewController Modal `UIViewController`.
 */
- (void)adView:(YMAAdView *)adView didDismissScreen:(nullable UIViewController *)viewController;

/**
 Notifies delegate when an impression was tracked.
 @param adView A reference to the object of the YMAAdView class that invoked the method.
 @param impressionData Ad impression-level revenue data.
 */
- (void)adView:(YMAAdView *)adView didTrackImpressionWithData:(nullable id<YMAImpressionData>)impressionData;

@end

NS_ASSUME_NONNULL_END
