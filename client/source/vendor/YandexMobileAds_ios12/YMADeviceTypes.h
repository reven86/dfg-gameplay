/*
 * Version for iOS © 2015–2021 YANDEX
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at https://yandex.com/legal/mobileads_sdk_agreement/
 */

#import <Foundation/Foundation.h>

/**
 Device types.
 */
typedef NS_OPTIONS(NSUInteger, YMADeviceType) {
/**
 The device is not defined.
 */
    YMADeviceTypeNone = 0,
/**
 Real device.
 */
    YMADeviceTypeHardware = 1 << 0,
/**
 A device simulator.
 */
    YMADeviceTypeSimulator  = 1 << 1
};
