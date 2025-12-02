/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>




NS_ASSUME_NONNULL_BEGIN

typedef void(^YMABidderTokenLoaderCompletion)(NSString * _Nullable bidderToken);

/**
 Class contains logic that creates a bidder token to be used in the mediation.
 Bidder token is generated asynchronously.
 */

@interface YMABidderTokenLoader : NSObject



/**
 Creates bidder token and dispatches results into completionHandler.
 @param completionHandler bidder token loading handler.
 */
- (void)loadBidderTokenWithCompletionHandler:(YMABidderTokenLoaderCompletion)completionHandler;

@end

NS_ASSUME_NONNULL_END
