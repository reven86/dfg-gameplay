/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

@class YMAAudioSessionManager;

NS_ASSUME_NONNULL_BEGIN

/**
 The protocol defines the methods of a delegate that monitors the audio session.
 */
@protocol YMAAudioSessionManagerDelegate <NSObject>

/**
 Notifies that the audio will start playing.
 @param audioSessionManager A reference to the object of the YMAAudioSessionManager class that invoked the method.
 */
- (void)audioSessionManagerWillPlayAudio:(YMAAudioSessionManager *)audioSessionManager;

/**
 Notifies that the audio did stop playing.
 @param audioSessionManager A reference to the object of the YMAAudioSessionManager class that invoked the method.
 */
- (void)audioSessionManagerDidStopPlayingAudio:(YMAAudioSessionManager *)audioSessionManager;

@end

NS_ASSUME_NONNULL_END
