/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>


NS_ASSUME_NONNULL_BEGIN

/**
 This class is responsible for the banner size.
 */
@interface YMAAdSize : NSObject

/**
 The initial size of the banner.
 @discussion The actual size of the banner is determined when
 calling the [YMAAdViewDelegate adViewDidLoad:] method of the YMAAdViewDelegate class.
 */
@property (nonatomic, assign, readonly) CGSize size;



- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

/**
 Creates an object of the YMAAdSize class with the specified maximum height and width of the banner.
 @param size Maximum size available for a banner.
 @return An object of the YMAAdSize class with the fixed size.
 */
+ (instancetype)fixedSizeWithCGSize:(CGSize)size __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Creates an object of the YMAAdSize class with the specified maximum height and width of the banner.
 @warning If the actual size of the banner is less than the size of the container,
 an adaptive layer is drawn under the banner.
 @param size Maximum size available for a banner.
 @return An object of the YMAAdSize class with the specified maximum size of the banner.
 */
+ (instancetype)flexibleSizeWithCGSize:(CGSize)size __deprecated_msg("Use inlineSizeWithWidth:maxHeight or stickySizeWithContainerWidth: instead of flexibleSizeWithCGSize:. This method will be removed in the directly following major SDK release.*");

/**
 Creates an object of the YMAAdSize class with the specified maximum height and width of the banner.
 @warning If the actual size of the banner is less than the size of the container,
 an adaptive layer is drawn under the banner.
 @param width Width of the banner.
 @param height Maximum height available for an adaptive banner.
 @return An object of the YMAAdSize class with the specified maximum size of the banner.
 */
+ (instancetype)inlineSizeWithWidth:(CGFloat)width maxHeight:(CGFloat)height;

/**
 Creates an object of the YMAAdSize class with the specified width of the banner.
 @discussion Returns sticky banner size with the given width.
 @param width Width of the banner.
 @return An object of the YMAAdSize class with the specified width of the sticky banner.
 */
+ (instancetype)stickySizeWithContainerWidth:(CGFloat)width;

@end

NS_ASSUME_NONNULL_END

#pragma mark - Fixed sizes

/**
 Banner sizes.
 */

/**
 Banner sized 320x50 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_320x50 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 320x100 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_320x100 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 320x250 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_300x250 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 320x300 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_300x300 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 240x400 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_240x400 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 400x240 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_400x240 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");

/**
 Banner sized 728x90 logical pixels.
 */
extern CGSize const YMAAdSizeBanner_728x90 __deprecated_msg("Use inlineSizeWithWidth:maxHeight: or stickySizeWithContainerWidth: instead of fixedSizeWithCGSize:. Fixed YMAAdSize API will be removed in the directly following major SDK release.");
