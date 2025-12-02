/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

@class YMANativeBulkAdLoader;
@protocol YMANativeAd;

NS_ASSUME_NONNULL_BEGIN

@protocol YMANativeBulkAdLoaderDelegate <NSObject>

/**
 Notifies that a native ads are loaded.
 @param nativeBulkAdLoader The loader that sends the message.
 @param ads Loaded native ads.
 */
- (void)nativeBulkAdLoader:(YMANativeBulkAdLoader *)nativeBulkAdLoader didLoadAds:(NSArray<id<YMANativeAd>> *)ads;

/**
 Notifies that the ads failed to load.
 @param nativeBulkAdLoader The loader that sends the message.
 @param error Information about the error (for details, see YMAAdErrorCode).
 */
- (void)nativeBulkAdLoader:(YMANativeBulkAdLoader *)nativeBulkAdLoader didFailLoadingWithError:(NSError *)error;

@end

NS_ASSUME_NONNULL_END
