export MY_DIR=~/Downloads/icu
cd $MY_DIR
cd build_icu_simulator/
DEVROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer
SDKROOT=$DEVROOT/SDKs/iPhoneSimulator6.1.sdk
SYSROOT=$SDKROOT
ICU_FLAGS="-I$ICU_PATH/source/common/ -I$ICU_PATH/source/tools/tzcode/ -O3 \
-DU_USING_ICU_NAMESPACE=1 -fno-short-enums -DU_HAVE_NL_LANGINFO_CODESET=0 \
-D__STDC_INT64__ -DU_TIMEZONE=0 -DUCONFIG_NO_LEGACY_CONVERSION=1 \
-DUCONFIG_NO_BREAK_ITERATION=1 -DUCONFIG_NO_COLLATION=1 \
-DUCONFIG_NO_TRANSLITERATION=0 -DUCONFIG_NO_REGULAR_EXPRESSIONS=1"
export CPPFLAGS="-I$SDKROOT/usr/include/ -I$SDKROOT/usr/include/ -I./include/ \
-miphoneos-version-min=2.2 $ICU_FLAGS -pipe -arch i386 -no-cpp-precomp \
-isysroot $SDKROOT"
export LDFLAGS="-arch i386 -L$SDKROOT/usr/lib/ -isysroot $SDKROOT \
-Wl,-dead_strip -miphoneos-version-min=2.0"
ICU_PATH=$MY_DIR
sh $ICU_PATH/source/configure --host=i686-apple-darwin11 --enable-static --disable-shared -with-cross-build=$ICU_PATH/build_icu_mac --prefix=$PWD/icu_build
make -j4
make install