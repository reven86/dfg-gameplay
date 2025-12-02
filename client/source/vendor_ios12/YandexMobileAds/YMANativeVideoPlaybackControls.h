/*
 * Version for iOS © 2015-2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>


@protocol YMANativeVideoPlaybackProgressControl;
@protocol YMANativeVideoPlaybackMuteControl;

NS_ASSUME_NONNULL_BEGIN

/**
 Class with playback controls.
 */
@interface YMANativeVideoPlaybackControls : NSObject



- (instancetype)init NS_UNAVAILABLE;

/**
 Initializes an object of the YMANativeVideoPlaybackControls class with progress control and mute control.
 @param progressControl An object of the `UIView` class and conform to protocol `YMANativeVideoPlaybackProgressControl`.
 @param muteControl An object of the `UIView` class and conform to protocol `YMANativeVideoPlaybackMuteControl`.
 @return An object of the YMANativeVideoPlaybackControls class with progress control and mute control.
 */
- (instancetype)initWithProgressControl:(nullable UIView<YMANativeVideoPlaybackProgressControl> *)progressControl
                            muteControl:(nullable UIView<YMANativeVideoPlaybackMuteControl> *)muteControl;

@end

NS_ASSUME_NONNULL_END
