/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

/**
 Common domain for `Yandex Mobile Ads` library errors.
 */
extern NSString *const kYMAAdsErrorDomain;

/**
 Error description.
 */
typedef NS_ENUM(NSInteger, YMAAdErrorCode) {
    /**
     The `AdUnitID` was omitted when loading the ad.
     */
    YMAAdErrorCodeEmptyAdUnitID,
    /**
     An invalid `Application ID` was specified.
     */
    YMAAdErrorCodeInvalidUUID,
    /**
     The `AdUnitID` specified when loading the ad wasn't found.
     */
    YMAAdErrorCodeNoSuchAdUnitID,
    /**
     The ad was loaded successfully, but there aren't any available ads to display.
     */
    YMAAdErrorCodeNoFill,
    /**
     Unexpected server response when loading the ad.
     */
    YMAAdErrorCodeBadServerResponse,
    /**
     The ad size in the request does not match the ad size specified in the Partner interface for this ad block.
     */
    YMAAdErrorCodeAdSizeMismatch,
    /**
     The ad type in the request does not match the ad type specified in the Partner interface for this ad block.
     */
    YMAAdErrorCodeAdTypeMismatch,
    /**
     The service is temporarily unavailable. Try sending the request again later.
     */
    YMAAdErrorCodeServiceTemporarilyNotAvailable,
    /**
     A full-screen ad can be shown only once.
     */
    YMAAdErrorCodeAdHasAlreadyBeenPresented,
    /**
     ViewController passed for presenting full-screen ad is nil
     */
    YMAAdErrorCodeNilPresentingViewController,
    /**
     Incorrect fullscrseen view
     */
    YMAAdErrorCodeIncorrectFullscreenView,
    /**
     Invalid sdk configuration
     */
    YMAAdErrorCodeInvalidSDKConfiguration,
};
