/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>


NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeAd;
@protocol YMAImpressionData;

@protocol YMANativeAdDelegate <NSObject>

@optional

/**
 `UIViewController` object that is used for showing a modal controller
 in response to the user's interaction with the banner.
 @return `UIViewController` object that is used for showing a modal controller
 in response to the user's interaction with the banner.
 */
- (nullable UIViewController *)viewControllerForPresentingModalView;

/**
 Notifies that the user has clicked on the ad.
 @param ad A reference to the object that invoked the method.
 */
- (void)nativeAdDidClick:(id<YMANativeAd>)ad;

/**
 Notifies that the user switched from the ad to an app (for example, the browser).
 @param ad A reference to the object that invoked the method.
 */
- (void)nativeAdWillLeaveApplication:(id<YMANativeAd>)ad;

/**
 Notifies that the ad will show the modal `UIViewController`
 in response to the user interacting with the banner.
 @param ad A reference to the object that invoked the method.
 @param viewController Modal `UIViewController`.
 */
- (void)nativeAd:(id<YMANativeAd>)ad willPresentScreen:(nullable UIViewController *)viewController;

/**
 Notifies that the ad finished showing the modal `UIViewController`
 in response to the user interacting with the banner.
 @param ad A reference to the object that invoked the method.
 @param viewController Modal `UIViewController`.
 */
- (void)nativeAd:(id<YMANativeAd>)ad didDismissScreen:(nullable UIViewController *)viewController;

/**
 Notifies delegate when an impression was tracked.
 @param ad A reference to the object that invoked the method.
 @param impressionData Ad impression-level revenue data.
 */
- (void)nativeAd:(id<YMANativeAd>)ad didTrackImpressionWithData:(nullable id<YMAImpressionData>)impressionData;

/**
 Notifies that the user has chosen a reason for closing the ad and the ad must be hidden.
 @warning Advertising will not be hidden.
 The developer must determine what to do with the ad after the reason for closing it is chosen.
 @param ad A reference to the object that invoked the method.
 */
- (void)closeNativeAd:(id<YMANativeAd>)ad;

@end

NS_ASSUME_NONNULL_END
