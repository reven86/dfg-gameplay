/*
 * Version for iOS © 2015-2022 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

@protocol YMANativeVideoPlaybackMuteControl;

NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeVideoPlaybackMuteControlDelegate <NSObject>

/**
 Notifies that the user has clicked on mute control.
 @param muteControl A reference to the object that invoked the method.
 @param isMuted A flag for representing current mute control state.
 */
- (void)muteControl:(id<YMANativeVideoPlaybackMuteControl>)muteControl didChangeIsMuted:(BOOL)isMuted;

@end

NS_ASSUME_NONNULL_END
