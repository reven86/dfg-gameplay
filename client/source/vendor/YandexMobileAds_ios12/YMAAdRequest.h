/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <CoreLocation/CoreLocation.h>
#import <Foundation/Foundation.h>
#import "YMAAdTheme.h"

NS_ASSUME_NONNULL_BEGIN

/**
 A class with data for a targeted ad request.
 */
@interface YMAAdRequest : NSObject <NSCopying, NSMutableCopying>

/**
 User age.
 */
@property (nonatomic, strong, readonly, nullable) NSNumber *age;

/**
 The search query that the user entered in the app.
 */
@property (nonatomic, copy, readonly, nullable) NSString *contextQuery;

/**
 List of tags. Matches the context in which the ad will be displayed.
 */
@property (nonatomic, copy, readonly, nullable) NSArray<NSString *> *contextTags;

/**
 The gender of the user. For a list of values, see the YMAGender section.
 */
@property (nonatomic, copy, readonly, nullable) NSString *gender;

/**
 User location.
 */
@property (nonatomic, copy, readonly, nullable) CLLocation *location;

/**
 Preferred ad theme.
 */
@property (nonatomic, assign, readonly) YMAAdTheme adTheme;

/**
 Bidding data for ad loading from mediation.
 */
@property (nonatomic, copy, readonly, nullable) NSString *biddingData;

/**
 Header bidding data for ad loading from mediation.
 */
@property (nonatomic, copy, readonly, nullable) NSString *headerBiddingData;

/**
 A set of arbitrary input parameters.
 */
@property (nonatomic, copy, readonly, nullable) NSDictionary<NSString *, NSString*> *parameters;

@end

/**
 An editable version of the YMAAdRequest class with data for a targeted ad request.
 */
@interface YMAMutableAdRequest : YMAAdRequest

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
 Header bidding data for ad loading from mediation, that contains bid id.
 */
@property (nonatomic, copy, nullable) NSString *headerBiddingData;

/**
 A set of arbitrary input parameters.
 */
@property (nonatomic, copy, nullable) NSDictionary<NSString *, NSString *> *parameters;

@end

NS_ASSUME_NONNULL_END
