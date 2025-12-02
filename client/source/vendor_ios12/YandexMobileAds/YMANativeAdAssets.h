/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>



@class YMANativeAdImage;
@class YMANativeAdMedia;



/**
 Class with native ad assets.
 */
@interface YMANativeAdAssets : NSObject

/**
 Age restrictions.
 */
@property (nonatomic, copy, nullable) NSString *age;

/**
 The main ad text.
 */
@property (nonatomic, copy, nullable) NSString *body;

/**
 The call to action.
 */
@property (nonatomic, copy, nullable) NSString *callToAction;


/**
 The domain.
 */
@property (nonatomic, copy, nullable) NSString *domain;

/**
 The web page's favicon.
 */
@property (nonatomic, strong, nullable) YMANativeAdImage *favicon;

/**
 Reports about the necessity to show `feedbackButton`
 */
@property (nonatomic, assign) BOOL feedbackAvailable;

/**
 The app's icon.
 */
@property (nonatomic, strong, nullable) YMANativeAdImage *icon;

/**
 The main image.
 */
@property (nonatomic, strong, nullable) YMANativeAdImage *image;

/**
 Ad media.
 */
@property (nonatomic, strong, nullable) YMANativeAdMedia *media;

/**
 The price of the advertised app.
 */
@property (nonatomic, copy, nullable) NSString *price;

/**
 The app's rating.
 */
@property (nonatomic, strong, nullable) NSNumber *rating;

/**
 The number of app reviews.
 */
@property (nonatomic, copy, nullable) NSString *reviewCount;

/**
 Data on the ad network.
 */
@property (nonatomic, copy, nullable) NSString *sponsored;

/**
 The ad title.
 */
@property (nonatomic, copy, nullable) NSString *title;

/**
 The warning.
 */
@property (nonatomic, copy, nullable) NSString *warning;



@end
