/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

/* Yandex Mobile Native Ads error domain. */
extern NSString *const kYMANativeAdErrorDomain;

/**
 Error description.
 */
typedef NS_ENUM(NSInteger, YMANativeErrorCode) {
    /**
     Occurs when binding an ad if the `UIView` is omitted for a required asset.
     */
    YMANativeAdErrorCodeNoViewForAsset,
    /**
     Occurs when the nil value is passed to the ad binding method as the `View`.
     */
    YMANativeAdErrorCodeInvalidViewForBinding
};
