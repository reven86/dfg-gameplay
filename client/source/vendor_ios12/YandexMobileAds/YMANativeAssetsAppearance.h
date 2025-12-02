/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

@class YMASizeConstraint;

/**
 Object sizing constraint.
 */
typedef NS_ENUM(NSInteger, YMASizeConstraintType) {
    /**
     Fixed width or height.
     */
    YMASizeConstraintTypeFixed,
    /**
     The ratio of the size of something to the size of the entire contents.
     The actual size of the content is not considered.
     */
    YMASizeConstraintTypeFixedBannerRatio,
    /**
     The ratio of the size of something to the size of the entire content,
     but no more than the actual size of the content.
     */
    YMASizeConstraintTypePreferredBannerRatio
};

/**
 A class with fixed settings for the standard text appearance: font name, size, and color.
 @discussion If you want to edit the settings of the standard appearance,
 use the YMAMutableLabelAppearance class.
 */
@interface YMALabelAppearance : NSObject <NSCopying, NSMutableCopying>

/**
 Font settings: name and size.
 */
@property (nonatomic, strong, readonly) UIFont *font;

/**
 Text color.
 */
@property (nonatomic, strong, readonly) UIColor *textColor;

/**
 Creates an object of the YMALabelAppearance class with the text settings.
 @param font Font settings: name and size.
 @param textColor Text color.
 @return The configured text appearance (the specified font and color).
 */
+ (instancetype)appearanceWithFont:(UIFont *)font
                         textColor:(UIColor *)textColor;

@end

/**
 Class with editable text settings.
 */
@interface YMAMutableLabelAppearance : YMALabelAppearance

/**
 Font settings: name and size.
 */
@property (nonatomic, strong) UIFont *font;

/**
 Text color.
 */
@property (nonatomic, strong) UIColor *textColor;

@end

/**
 A class with fixed settings for the standard button appearance.
 @discussion If you want to edit the standard appearance settings,
 use the YMAMutableNativeTemplateAppearance class.
 */
@interface YMAButtonAppearance : NSObject <NSCopying, NSMutableCopying>

/**
 Settings for the button label.
 */
@property (nonatomic, copy, readonly) YMALabelAppearance *textAppearance;

/**
 Label color when the button is clicked.
 */
@property (nonatomic, strong, readonly) UIColor *highlightedTextColor;

/**
 The color of the button background in its normal state.
 */
@property (nonatomic, strong, readonly) UIColor *normalColor;

/**
 The color of the button background when clicked.
 */
@property (nonatomic, strong, readonly) UIColor *highlightedColor;

/**
 The color of the button border.
 */
@property (nonatomic, strong, readonly) UIColor *borderColor;

/**
 The thickness of the button border.
 */
@property (nonatomic, assign, readonly) CGFloat borderWidth;

/**
 Creates an object of the YMAButtonAppearance class,
 i.e., a button with the specified appearance (label font, label color, border color, and other settings).
 @param textAppearance Settings for the button label.
 @param normalColor The color of the button background in its normal state.
 @param highlightedColor The color of the button background when clicked.
 @param borderColor The color of the button border.
 @param borderWidth The thickness of the button border.
 @return Configured button appearance.
 */
+ (instancetype)appearanceWithTextAppearance:(YMALabelAppearance *)textAppearance
                                 normalColor:(UIColor *)normalColor
                            highlightedColor:(UIColor *)highlightedColor
                                 borderColor:(UIColor *)borderColor
                                 borderWidth:(CGFloat)borderWidth;

/**
 Creates an object of the YMAButtonAppearance class,
 i.e., a button with the specified appearance (label font, label color, border color, and other settings).
 @param textAppearance Settings for the button label.
 @param highlightedTextColor Label color when the button is clicked.
 @param normalColor The color of the button background in its normal state.
 @param highlightedColor The color of the button background when clicked.
 @param borderColor The color of the button border.
 @param borderWidth The thickness of the button border.
 @return Configured button appearance.
 */
+ (instancetype)appearanceWithTextAppearance:(YMALabelAppearance *)textAppearance
                        highlightedTextColor:(UIColor *)highlightedTextColor
                                 normalColor:(UIColor *)normalColor
                            highlightedColor:(UIColor *)highlightedColor
                                 borderColor:(UIColor *)borderColor
                                 borderWidth:(CGFloat)borderWidth;

@end

/**
 Class with editable settings for the standard button appearance.
 */
@interface YMAMutableButtonAppearance : YMAButtonAppearance

/**
 Settings for the button label.
 */
@property (nonatomic, copy) YMALabelAppearance *textAppearance;

/**
 Label color when the button is clicked.
 */
@property (nonatomic, strong) UIColor *highlightedTextColor;

/**
 The color of the button background in its normal state.
 */
@property (nonatomic, strong) UIColor *normalColor;

/**
 The color of the button background when clicked.
 */
@property (nonatomic, strong) UIColor *highlightedColor;

/**
 The color of the button border.
 */
@property (nonatomic, strong) UIColor *borderColor;

/**
 The thickness of the button border.
 */
@property (nonatomic, assign) CGFloat borderWidth;

@end

/**
 A class with fixed settings for the standard rating appearance.
 @discussion If you want to edit the settings of the standard appearance,
 use the YMAMutableRatingAppearance class.
 */
@interface YMARatingAppearance : NSObject <NSCopying, NSMutableCopying>

/**
 The outline color of an empty (unfilled) star.
 */
@property (nonatomic, strong, readonly) UIColor *emptyStarColor;

/**
 The color of a filled star.
 */
@property (nonatomic, strong, readonly) UIColor *filledStarColor;

/**
 The preferred size of a star.
 @warning If the use of the preferred size makes the rating too large for the space reserved for it,
 the maximum allowed size is used instead.
 */
@property (nonatomic, assign, readonly) CGFloat preferredStarSize;

/**
 Creates an object of the YMARatingAppearance class,
 i.e., a rating with the specified appearance (outline color, size, and other settings).
 @param emptyStarColor The outline color of an empty (unfilled) star.
 @param filledStarColor The color of a filled star.
 @param starSize The preferred size of a star.
 @return Configured rating appearance.
 */
+ (instancetype)appearanceWithEmptyStarColor:(UIColor *)emptyStarColor
                             filledStarColor:(UIColor *)filledStarColor
                                    starSize:(CGFloat)starSize;

@end

/**
 A class with editable settings for the standard rating appearance.
 */
@interface YMAMutableRatingAppearance : YMARatingAppearance

/**
 The outline color of an empty (unfilled) star.
 */
@property (nonatomic, strong) UIColor *emptyStarColor;

/**
 The color of a filled star.
 */
@property (nonatomic, strong) UIColor *filledStarColor;

/**
 The preferred size of a star.
 @warning If the use of the preferred size makes the rating too large for the space reserved for it,
 the maximum allowed size is used instead.
 */
@property (nonatomic, assign) CGFloat preferredStarSize;

@end

/**
 Class with standard image appearance settings (non-editable).
 @discussion If you want to edit the settings of the standard appearance,
 use the YMAMutableRatingAppearance class.
 */
@interface YMAImageAppearance : NSObject <NSCopying, NSMutableCopying>

/**
 Limits the width of the image.
 */
@property (nonatomic, strong, readonly) YMASizeConstraint *widthConstraint;

/**
 Creates an object of the YMAImageAppearance class,
 i.e., an image with the specified sizing constraint.
 @param widthConstraint Limits the width of the image.
 @return Configured image appearance.
 */
+ (instancetype)appearanceWithWidthConstraint:(YMASizeConstraint *)widthConstraint;

@end

/**
 Class with editable settings for the standard image appearance.
 */
@interface YMAMutableImageAppearance : YMAImageAppearance

/**
 Limits the width of the image.
 */
@property (nonatomic, strong) YMASizeConstraint *widthConstraint;

@end

/**
 A class with object sizing constraints.
 @discussion If you want to edit the object sizing constraints, use the YMAMutableSizeConstraint class.
 */
@interface YMASizeConstraint : NSObject <NSCopying, NSMutableCopying>

/**
 Object sizing constraint.
 @discussion Acceptable values are listed in YMASizeConstraintType enum.
 */
@property (nonatomic, assign, readonly) YMASizeConstraintType type;

/**
 The constraint value. The meaning of the value depends on the `type` property.
 */
@property (nonatomic, assign, readonly) CGFloat value;

/**
 Creates an object of the YMASizeConstraint class, i.e., the object sizing constraint.
 @param type Object sizing constraint. Acceptable values are listed in YMASizeConstraintType.
 @param value The constraint value. The meaning of the value depends on the `type` property.
 @return The specified object sizing constraint.
 */
+ (instancetype)constraintWithType:(YMASizeConstraintType)type value:(CGFloat)value;

@end

/**
 A class with editable object sizing constraints.
 */
@interface YMAMutableSizeConstraint : YMASizeConstraint

/**
 Object sizing constraint.
 @discussion Acceptable values are listed in YMASizeConstraintType.
 */
@property (nonatomic, assign) YMASizeConstraintType type;

/**
 The constraint value. The meaning of the value depends on the `type` property.
 */
@property (nonatomic, assign) CGFloat value;

@end

NS_ASSUME_NONNULL_END
