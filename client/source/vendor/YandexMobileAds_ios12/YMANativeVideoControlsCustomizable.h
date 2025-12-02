/*
 * Version for iOS © 2015-2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class YMANativeVideoPlaybackControls;

/**
 Objects that implement this protocol allow to set custom video playback controls;
 */
@protocol YMANativeVideoControlsCustomizable <NSObject>

/**
 Sets an NativeVideoPlaybackControls
 - Parameter videoPlaybackControls: Video playback controls.
 */
- (void)setVideoPlaybackControls:(YMANativeVideoPlaybackControls *)videoPlaybackControls;

@end

NS_ASSUME_NONNULL_END
