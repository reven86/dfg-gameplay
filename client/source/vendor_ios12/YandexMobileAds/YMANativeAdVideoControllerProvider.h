/*
 * Version for iOS © 2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class YMANativeAdVideoController;

/**
 This protocol allows to get NativeAdVideoController from NativeAd
 */
@protocol YMANativeAdVideoControllerProvider <NSObject>

/**
 VideoController provides playback control for ad video.
 */
@property (nonatomic, strong, readonly, nullable) YMANativeAdVideoController *videoController;

@end

NS_ASSUME_NONNULL_END
