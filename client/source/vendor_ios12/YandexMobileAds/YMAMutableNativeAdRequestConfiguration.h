/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import "YMANativeAdRequestConfiguration.h"


NS_ASSUME_NONNULL_BEGIN

/**
 An editable version of the YMANativeAdRequestConfiguration class configuring native ad loading.
 */
@interface YMAMutableNativeAdRequestConfiguration : YMANativeAdRequestConfiguration

/**
 User age.
 */
@property (nonatomic, strong, nullable) NSNumber *age;

/**
 The search query that the user entered in the app.
 */
@property (nonatomic, copy, nullable) NSString *contextQuery;

/**
 List of tags. Matches the context in which the ad will be displayed.
 */
@property (nonatomic, copy, nullable) NSArray<NSString *> *contextTags;

/**
 The gender of the user. For a list of values, see the YMAGender section.
 */
@property (nonatomic, copy, nullable) NSString *gender;

/**
 User location.
 */
@property (nonatomic, copy, nullable) CLLocation *location;

/**
 Preferred ad theme.
 */
@property (nonatomic, assign) YMAAdTheme adTheme;

/**
 Bidding data for ad loading from mediation.
 */
@property (nonatomic, copy, nullable) NSString *biddingData;

/**
 A set of arbitrary input parameters.
 */
@property (nonatomic, copy, nullable) NSDictionary<NSString *, NSString*> *parameters;



/**
 Flag for automatic image loading. Acceptable values: `YES` – Load automatically; `NO` – Load manually.
 @warning If the app simultaneously stores links to a large number of ads,
 we recommend using manual image loading.
 */
@property (nonatomic, assign) BOOL shouldLoadImagesAutomatically;

@end

NS_ASSUME_NONNULL_END
