/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeAd;

/**
 This protocol is responsible for sending messages about image loading.
 */
@protocol YMANativeAdImageLoadingObserver <NSObject>

/**
 Notifies that the image is loaded successfully.
 @param ad A reference to the object that invoked the method.
 */
- (void)nativeAdDidFinishLoadingImages:(id<YMANativeAd>)ad;

@end

NS_ASSUME_NONNULL_END
