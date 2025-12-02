/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>
#import <YandexMobileAds/YMANativeAdType.h>



NS_ASSUME_NONNULL_BEGIN

@class YMANativeAdAssets;
@class YMANativeAdView;
@protocol YMANativeAdDelegate;
@protocol YMANativeAdImageLoadingObserver;





/**
 The protocol provides methods for managing an ad and getting the values of the ad assets.
 */
@protocol YMANativeAd <NSObject>

/**
 Type of native ad. For acceptable values, see YMANativeAdType.
 */
@property (nonatomic, assign, readonly) YMANativeAdType adType;

/**
 An object implementing the YMANativeAdDelegate protocol
 that receives events triggered by the user's interaction with the ad.
 */
@property (nonatomic, weak, nullable) id<YMANativeAdDelegate> delegate;

/**
 Adds an observer that receives notifications about the image loading process.
 @param observer The observer of the image loading process.
 */
- (void)addImageLoadingObserver:(id<YMANativeAdImageLoadingObserver>)observer;

/**
 Removes the observer that receives notifications about the image loading process.
 @param observer The observer of the image loading process.
 */
- (void)removeImageLoadingObserver:(id<YMANativeAdImageLoadingObserver>)observer;

/**
 An object with ad assets.
 @return Returns an object containing the ad assets.
 */
- (YMANativeAdAssets *)adAssets;

/**
 Any string in the ad (set in the Partner interface).
 @warning This property is only used for working with ADFOX.
 */
@property (nonatomic, copy, readonly, nullable) NSString *info;

/**
 An array with nested ads.
 */
@property (nonatomic, copy, readonly) NSArray<id<YMANativeAd>> *ads;

/**
 Loads images if manual loading is selected.
 @warning All images are cached, but they can be deleted at any time,
 so you need to call this method before every ad impression.
 */
- (void)loadImages;

/**
 Sets values of all ad assets to native ad view, installs impression and click handlers.
 @param adView  `YMANativeAdView` with views for ad assets.
 @param error Binding error. @see YMANativeAdErrors.h for error codes.
 @return YES if binding succeeded, otherwise NO.
 */
- (BOOL)bindWithAdView:(YMANativeAdView *)adView error:(NSError **)error;

/**
 Configures views for native ads displlay in slider.
 @param sliderView Root slider ad view with general assets
 @param error Binding error. @see YMANativeAdErrors.h for error codes.
 @return YES if binding succeeded, otherwise NO.
 */
- (BOOL)bindAdToSliderView:(YMANativeAdView *)sliderView
                     error:(NSError **)error;





@end

NS_ASSUME_NONNULL_END
