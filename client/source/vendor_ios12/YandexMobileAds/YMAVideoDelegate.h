/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

@class YMAVideoController;

NS_ASSUME_NONNULL_BEGIN

@protocol YMAVideoDelegate <NSObject>

@optional

/**
 Notifies delegate when video finished playing.
 @param videoController Video controller related to video.
 */
- (void)videoControllerDidFinishPlayingVideo:(YMAVideoController *)videoController;

@end

NS_ASSUME_NONNULL_END
