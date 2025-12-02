/*
 * Version for iOS © 2015-2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

@protocol YMANativeVideoPlaybackMuteControlDelegate;

NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeVideoPlaybackMuteControl <NSObject>

/**
 Delegate is notified about video playback mute control events.
 */
@property (nonatomic, weak, nullable) id<YMANativeVideoPlaybackMuteControlDelegate> delegate;

/**
 Configures a mute control with specified isMuted flag.
 @param isMuted Flag for setting mute/unmute state.
 */
- (void)configureWithIsMuted:(BOOL)isMuted;

@end

NS_ASSUME_NONNULL_END
