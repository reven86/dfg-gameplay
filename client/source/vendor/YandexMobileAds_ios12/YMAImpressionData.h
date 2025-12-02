/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 A protocol with impression-level revenue data.
 */
@protocol YMAImpressionData <NSObject, NSCoding>

/**
 A raw impression-level revenue data, string with json.
 */
@property (nonatomic, strong, readonly) NSString *rawData;

@end

NS_ASSUME_NONNULL_END
