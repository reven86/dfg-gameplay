#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@protocol YMANativeAd;
@protocol YMARating;
@class YMANativeMediaView;

NS_ASSUME_NONNULL_BEGIN

/**
 This class is responsible for creating the layout of an native ad without using a template.
 */
@interface YMANativeAdView : UIView

/**
 `UILabel` for information about age restrictions.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *ageLabel;

/**
 `UILabel` for the main ad text.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *bodyLabel;

/**
 `UIButton` with a call to action.
 */
@property (nonatomic, strong, nullable) IBOutlet UIButton *callToActionButton;

/**
 `UILabel` for domain data.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *domainLabel;

/**
 `UIImageView` for the favicon.
 */
@property (nonatomic, strong, nullable) IBOutlet UIImageView *faviconImageView;

/**
 `UIButton` for handling reasons for ad closing.
 */
@property (nonatomic, strong, nullable) IBOutlet UIButton *feedbackButton;

/**
 `UIImageView` for the icon.
 */
@property (nonatomic, strong, nullable) IBOutlet UIImageView *iconImageView;

/**
 `NativeMediaView` for the ad's media.
 */
@property (nonatomic, strong, nullable) IBOutlet YMANativeMediaView *mediaView;

/**
 `UILabel` for data on the price of the advertised app.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *priceLabel;

/**
 `UIView` that implements the `Rating` protocol for data on the app rating.
 */
@property (nonatomic, strong, nullable) IBOutlet UIView<YMARating> *ratingView;

/**
 `UILabel` for data on the number of app reviews.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *reviewCountLabel;

/**
 `UILabel` for information about the ad network.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *sponsoredLabel;

/**
 `UILabel` for the ad title.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *titleLabel;

/**
 `UILabel` for the warning.
 */
@property (nonatomic, strong, nullable) IBOutlet UILabel *warningLabel;

/**
 The ad object.
 */
@property (nonatomic, strong, readonly, nullable) id<YMANativeAd> ad;

@end

NS_ASSUME_NONNULL_END

