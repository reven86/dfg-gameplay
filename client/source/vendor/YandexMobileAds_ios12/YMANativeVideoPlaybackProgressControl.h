/*
 * Version for iOS © 2015-2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeVideoPlaybackProgressControl <NSObject>

/**
 Configures a progress control with position and duration.
 @param position Position of current OutStream ad.
 @param duration Duration of current OutStream ad.
 */
- (void)configureWithPosition:(NSTimeInterval)position duration:(NSTimeInterval)duration;

/**
 Resets a video progress.
 */
- (void)reset;

@end

NS_ASSUME_NONNULL_END
