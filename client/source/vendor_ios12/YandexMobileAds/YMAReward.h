/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 YMAReward represents reward given to the user.
 */
@protocol YMAReward <NSObject>

/**
 Amount rewarded to the user
 */
@property (nonatomic, assign, readonly) NSInteger amount;

/**
 Type of the reward.
 */
@property (nonatomic, copy, readonly) NSString *type;

@end

NS_ASSUME_NONNULL_END
