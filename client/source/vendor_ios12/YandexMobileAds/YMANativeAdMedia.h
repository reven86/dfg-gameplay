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
 YMANativeAdMedia represents media asset.
 */
@interface YMANativeAdMedia : NSObject

/**
 Media aspect ratio (width/height).
 */
@property (nonatomic, assign, readonly) CGFloat aspectRatio;

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;



@end

NS_ASSUME_NONNULL_END

