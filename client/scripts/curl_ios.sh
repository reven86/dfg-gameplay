export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/llvm-gcc-4.2"
export CFLAGS="-arch armv7 -arch armv7s -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
curl -O http://curl.haxx.se/download/curl-7.30.0.tar.gz
tar -xzf curl-7.30.0.tar.gz
cd curl-7.30.0
export CFLAGS="-arch armv7 -arch armv7s -arch i386 -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -arch i386 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin
export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/llvm-gcc-4.2"
export CFLAGS="-arch armv7 -arch armv7s -arch i386 -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
export LDFLAGS="-arch armv7 -arch armv7s -arch i386 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin
