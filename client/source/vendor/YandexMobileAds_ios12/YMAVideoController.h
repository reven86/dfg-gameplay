/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>


@protocol YMAVideoDelegate;

NS_ASSUME_NONNULL_BEGIN

/**
 YMAVideoController manages video content of the ad.
 */
@interface YMAVideoController : NSObject

/**
 Delegate is notified about video playback events.
 */
@property (nonatomic, weak, nullable) id<YMAVideoDelegate> delegate;



@end

NS_ASSUME_NONNULL_END
