export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_ios/
ICU_PATH=$MY_DIR
DEVROOT=/Applications/Xcode.app/Contents/Developer
SDKROOT=$DEVROOT/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk
SYSROOT=$SDKROOT
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-DU_USING_ICU_NAMESPACE=1 -fno-short-enums \
-DU_HAVE_NL_LANGINFO_CODESET=0 -D__STDC_INT64__ -DU_TIMEZONE=0 \
-DUCONFIG_NO_LEGACY_CONVERSION=1 -DUCONFIG_NO_BREAK_ITERATION=1 \
-DUCONFIG_NO_COLLATION=1 -DUCONFIG_NO_TRANSLITERATION=0 \
-DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CC="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export CXX="$DEVROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -arch arm64 -pipe -no-cpp-precomp -isysroot $SDKROOT"
export LDFLAGS="-L$SDKROOT/usr/lib/ -isysroot $SDKROOT -Wl,-dead_strip \
-miphoneos-version-min=2.0 -arch arm64"
sh $ICU_PATH/source/configure --host=arm-apple-darwin --enable-static --disable-shared -with-cross-build=$MY_DIR/build_icu_mac --prefix=$PWD/icu_build
make -j4
make install