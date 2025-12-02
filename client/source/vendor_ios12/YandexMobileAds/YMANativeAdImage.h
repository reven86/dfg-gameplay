/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <UIKit/UIKit.h>


NS_ASSUME_NONNULL_BEGIN

/**
 This class describes an image as a native ad asset.
 @discussion The image sizes are always available, but the images themselves are only available after loading.
 */
@interface YMANativeAdImage : NSObject




/**
 Image size.
 */
@property (nonatomic, assign, readonly) CGSize size;

/**
 Loaded image. The image is available only after loading.
 */
@property (nonatomic, strong, readonly, nullable) UIImage *imageValue;




@end

NS_ASSUME_NONNULL_END
