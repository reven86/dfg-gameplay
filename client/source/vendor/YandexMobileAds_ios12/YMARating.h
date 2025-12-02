/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 The protocol sets and receives the rating value.
 */
@protocol YMARating <NSObject>

/**
 Sets the rating value in the range [0; 5].
 @param rating The rating value in the range [0; 5].
 */
- (void)setRating:(nullable NSNumber *)rating;

/**
 Returns the rating value in the range [0; 5].
 @return Rating value.
 */
- (nullable NSNumber *)rating;

@end

NS_ASSUME_NONNULL_END
