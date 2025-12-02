/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>
#import <YandexMobileAds/YMADeviceTypes.h>


NS_ASSUME_NONNULL_BEGIN

@class YMAAudioSessionManager;

/**
 This class allows you to set general SDK settings.
 */
@interface YMAMobileAds : NSObject

- (instancetype)init NS_UNAVAILABLE;

+ (instancetype)new NS_UNAVAILABLE;

/**
 Enables logging. By default, logging is disabled.
 */
+ (void)enableLogging;

/**
 Returns the SDK version in the X.YY format.
 @return The version of the SDK in X.YY format.
 */
+ (NSString *)SDKVersion;

/**
 Returns the audio session manager.
 @return The audio session manager.
 */
+ (YMAAudioSessionManager *)audioSessionManager;

/**
 The SDK automatically collects location data if the user allowed the app to track the location.
 This option is enabled by default.
 @param enabled Enables or disables collecting location data.
 */
+ (void)setLocationTrackingEnabled:(BOOL)enabled;

/**
 Enables/disables the incorrect integration indicator for native advertising.
 @discussion By default, the indicator of incorrect integration (for native ads) is enabled
 for the `YMADeviceTypeSimulator` device type (the types are listed in YMADeviceTypes).
 To disable the indicator, pass the `YMADeviceTypeNone` value.
 @warning The indicator is not displayed in apps installed from the AppStore,
 regardless of the `deviceType` parameter value.
 @param deviceType The type of a device for displaying the incorrect integration indicator.
 */
+ (void)enableVisibilityErrorIndicatorForDeviceType:(YMADeviceType)deviceType;

/**
 Set a value indicating whether user from GDPR region allowed to collect personal data
 which is used for analytics and ad targeting.
 If the value is set to `NO` personal data will not be collected. The default value is `NO`.

 @param consent `YES` if user provided consent to collect personal data, otherwise `NO`.
 */
+ (void)setUserConsent:(BOOL)consent;

/**
 Initializes the Yandex Mobile Ads SDK internal state.
 */
+ (void)initializeSDK;



@end

NS_ASSUME_NONNULL_END
