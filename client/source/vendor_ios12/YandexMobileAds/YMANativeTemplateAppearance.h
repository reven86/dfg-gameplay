/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

@class YMALabelAppearance;
@class YMAButtonAppearance;
@class YMARatingAppearance;
@class YMAImageAppearance;

typedef struct YMAHorizontalOffset {
    CGFloat left, right;
} YMAHorizontalOffset;

/**
 Class with the default (fixed) settings for the standard ad layout.
 If you want to edit the default settings of the standard appearance,
 use the YMAMutableNativeTemplateAppearance class.
 */
@interface YMANativeTemplateAppearance : NSObject <NSCopying, NSMutableCopying>

/**
 Width of the border.
 */
@property (nonatomic, assign, readonly) CGFloat borderWidth;

/**
 Color of the border.
 */
@property (nonatomic, strong, readonly) UIColor *borderColor;

/**
 Background color.
 */
@property (nonatomic, strong, readonly) UIColor *backgroundColor;

/**
 Horizontal (left and right) margins between the content and the edges of the ad.
 @discussion Measured in logical pixels.
 */
@property (nonatomic, assign, readonly) YMAHorizontalOffset contentPadding;

/**
 The horizontal margins around the image, relative to the ad content.
 @discussion The left margin is relative to the left edge of the content.
 The right margin is the gap between the right edge of the image and the left edge of the text.
 @warning It doesn't apply to large images that are placed under the main ad text
 and cover the entire width of the ad.
 */
@property (nonatomic, assign, readonly) YMAHorizontalOffset imageMargins;

/**
 Appearance settings for the text with age restrictions.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *ageAppearance;

/**
 Appearance settings for the main ad text.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *bodyAppearance;

/**
 Appearance settings for a call-to-action button (for example, the Install button).
 */
@property (nonatomic, copy, readonly, nullable) YMAButtonAppearance *callToActionAppearance;

/**
 Appearance settings for the domain text.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *domainAppearance;

/**
 Appearance settings for the favicon.
 */
@property (nonatomic, copy, readonly, nullable) YMAImageAppearance *faviconAppearance;

/**
 Appearance settings for the feedback.
 */
@property (nonatomic, copy, readonly, nullable) YMAImageAppearance *feedbackAppearance;

/**
 Appearance settings for the image.
 @warning It doesn't apply to large images that are placed under the main ad text
 and cover the entire width of the ad.
 */
@property (nonatomic, copy, readonly, nullable) YMAImageAppearance *imageAppearance;

/**
 Appearance settings for the rating.
 */
@property (nonatomic, copy, readonly, nullable) YMARatingAppearance *ratingAppearance;

/**
 Appearance settings for the ad network text.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *sponsoredAppearance;

/**
 Appearance settings for the ad title text.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *titleAppearance;

/**
 Appearance settings for the number of app reviews.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *reviewCountAppearance;

/**
 Appearance settings for the warning text.
 */
@property (nonatomic, copy, readonly, nullable) YMALabelAppearance *warningAppearance;

/**
 Creates an object with default settings for the standard design.
 @return Returns an object with the default settings for the standard design.
 */
+ (instancetype)defaultAppearance;

@end

/**
 Class with editable settings for the standard button design.
 */
@interface YMAMutableNativeTemplateAppearance : YMANativeTemplateAppearance

/**
 Width of the border.
 */
@property (nonatomic, assign) CGFloat borderWidth;

/**
 Color of the border.
 */
@property (nonatomic, strong) UIColor *borderColor;

/**
 Background color.
 */
@property (nonatomic, strong) UIColor *backgroundColor;

/**
 Horizontal (left and right) margins between the content and the edges of the ad.
 @discussion Measured in logical pixels.
 */
@property (nonatomic, assign) YMAHorizontalOffset contentPadding;

/**
 The horizontal margins around the image, relative to the ad content.
 @discussion The left margin is relative to the left edge of the content.
 The right margin is the gap between the right edge of the image and the left edge of the text.
 @warning It doesn't apply to large images that are placed under the main ad text
 and cover the entire width of the ad.
 */
@property (nonatomic, assign) YMAHorizontalOffset imageMargins;

/**
 Appearance settings for the text with age restrictions.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *ageAppearance;

/**
 Appearance settings for the main ad text.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *bodyAppearance;

/**
 Appearance settings for a call-to-action button (for example, the Install button).
 */
@property (nonatomic, copy, nullable) YMAButtonAppearance *callToActionAppearance;

/**
 Appearance settings for the domain text.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *domainAppearance;

/**
 Appearance settings for the favicon.
 */
@property (nonatomic, copy, nullable) YMAImageAppearance *faviconAppearance;

/**
 Appearance settings for the feedback.
 */
@property (nonatomic, copy, nullable) YMAImageAppearance *feedbackAppearance;

/**
 Appearance settings for the image.
 @warning It doesn't apply to large images that are placed under the main ad text
 and cover the entire width of the ad.
 */
@property (nonatomic, copy, nullable) YMAImageAppearance *imageAppearance;

/**
 Appearance settings for the rating.
 */
@property (nonatomic, copy, nullable) YMARatingAppearance *ratingAppearance;

/**
 Appearance settings for the ad network text.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *sponsoredAppearance;

/**
 Appearance settings for the ad title text.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *titleAppearance;

/**
 Appearance settings for the number of app reviews.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *reviewCountAppearance;

/**
 Appearance settings for the warning text.
 */
@property (nonatomic, copy, nullable) YMALabelAppearance *warningAppearance;

@end

NS_ASSUME_NONNULL_END
