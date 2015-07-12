#curl -O http://curl.haxx.se/download/curl-7.30.0.tar.gz
#tar -xzf curl-7.30.0.tar.gz
cd curl-7.30.0
DEVROOT=/Applications/Xcode.app/Contents/Developer
SDKROOT=$DEVROOT/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
export IPHONEOS_DEPLOYMENT_TARGET="4.3"
export CC="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CFLAGS="-arch arm64 -pipe -Os -gdwarf-2 -isysroot $SDKROOT"
export LDFLAGS="-arch arm64 -isysroot $SDKROOT"
./configure --disable-shared --enable-static --host=armv7-apple-darwin
make -j4